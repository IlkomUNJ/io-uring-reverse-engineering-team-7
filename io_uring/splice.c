// SPDX-License-Identifier: GPL-2.0
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/file.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/namei.h>
#include <linux/io_uring.h>
#include <linux/splice.h>

#include <uapi/linux/io_uring.h>

#include "io_uring.h"
#include "splice.h"

struct io_splice {
	struct file			*file_out;
	loff_t				off_out;
	loff_t				off_in;
	u64				len;
	int				splice_fd_in;
	unsigned int			flags;
	struct io_rsrc_node		*rsrc_node;
};

/*
 * __io_splice_prep() - Menyiapkan operasi splice
 * @req: pointer ke struktur io_kiocb yang menyimpan permintaan I/O
 * @sqe: pointer ke struktur io_uring_sqe yang berisi parameter permintaan
 *
 * Fungsi ini mempersiapkan operasi splice, memeriksa keabsahan flag, dan mengatur
 * parameter terkait panjang dan flag splice.
 */

static int __io_splice_prep(struct io_kiocb *req,
			    const struct io_uring_sqe *sqe)
{
	struct io_splice *sp = io_kiocb_to_cmd(req, struct io_splice);
	unsigned int valid_flags = SPLICE_F_FD_IN_FIXED | SPLICE_F_ALL;

	sp->len = READ_ONCE(sqe->len);
	sp->flags = READ_ONCE(sqe->splice_flags);
	if (unlikely(sp->flags & ~valid_flags))
		return -EINVAL;
	sp->splice_fd_in = READ_ONCE(sqe->splice_fd_in);
	sp->rsrc_node = NULL;
	req->flags |= REQ_F_FORCE_ASYNC;
	return 0;
}

/*
 * io_tee_prep() - Menyiapkan operasi tee untuk splice
 * @req: pointer ke struktur io_kiocb yang menyimpan permintaan I/O
 * @sqe: pointer ke struktur io_uring_sqe yang berisi parameter permintaan
 *
 * Fungsi ini digunakan untuk menyiapkan operasi tee, dengan memeriksa keberadaan offset
 * untuk input atau output. Jika ada, mengembalikan error.
 */
int io_tee_prep(struct io_kiocb *req, const struct io_uring_sqe *sqe)
{
	if (READ_ONCE(sqe->splice_off_in) || READ_ONCE(sqe->off))
		return -EINVAL;
	return __io_splice_prep(req, sqe);
}

/*
 * io_splice_cleanup() - Membersihkan sumber daya setelah operasi splice
 * @req: pointer ke struktur io_kiocb yang menyimpan permintaan I/O
 *
 * Fungsi ini digunakan untuk membebaskan node sumber daya yang digunakan selama operasi splice
 * jika ada node yang terkait.
 */

void io_splice_cleanup(struct io_kiocb *req)
{
	struct io_splice *sp = io_kiocb_to_cmd(req, struct io_splice);

	if (sp->rsrc_node)
		io_put_rsrc_node(req->ctx, sp->rsrc_node);
}

/*
 * io_splice_get_file() - Mendapatkan file untuk operasi splice
 * @req: pointer ke struktur io_kiocb yang menyimpan permintaan I/O
 * @issue_flags: flag yang digunakan untuk issue
 *
 * Fungsi ini akan mengembalikan file input untuk operasi splice, tergantung pada flag
 * SPLICE_F_FD_IN_FIXED. Jika flag tersebut tidak diatur, file diambil secara normal.
 * Jika flag diatur, maka file diambil melalui node sumber daya.
 */

static struct file *io_splice_get_file(struct io_kiocb *req,
				       unsigned int issue_flags)
{
	struct io_splice *sp = io_kiocb_to_cmd(req, struct io_splice);
	struct io_ring_ctx *ctx = req->ctx;
	struct io_rsrc_node *node;
	struct file *file = NULL;

	if (!(sp->flags & SPLICE_F_FD_IN_FIXED))
		return io_file_get_normal(req, sp->splice_fd_in);

	io_ring_submit_lock(ctx, issue_flags);
	node = io_rsrc_node_lookup(&ctx->file_table.data, sp->splice_fd_in);
	if (node) {
		node->refs++;
		sp->rsrc_node = node;
		file = io_slot_file(node);
		req->flags |= REQ_F_NEED_CLEANUP;
	}
	io_ring_submit_unlock(ctx, issue_flags);
	return file;
}

/*
 * io_tee() - Melakukan operasi tee antara dua file
 * @req: pointer ke struktur io_kiocb yang menyimpan permintaan I/O
 * @issue_flags: flag yang digunakan untuk issue
 *
 * Fungsi ini menjalankan operasi tee yang menyalin data dari file input ke file output.
 * Jika operasi berhasil, file input dibebaskan setelahnya.
 */

int io_tee(struct io_kiocb *req, unsigned int issue_flags)
{
	struct io_splice *sp = io_kiocb_to_cmd(req, struct io_splice);
	struct file *out = sp->file_out;
	unsigned int flags = sp->flags & ~SPLICE_F_FD_IN_FIXED;
	struct file *in;
	ssize_t ret = 0;

	WARN_ON_ONCE(issue_flags & IO_URING_F_NONBLOCK);

	in = io_splice_get_file(req, issue_flags);
	if (!in) {
		ret = -EBADF;
		goto done;
	}

	if (sp->len)
		ret = do_tee(in, out, sp->len, flags);

	if (!(sp->flags & SPLICE_F_FD_IN_FIXED))
		fput(in);
done:
	if (ret != sp->len)
		req_set_fail(req);
	io_req_set_res(req, ret, 0);
	return IOU_OK;
}

/*
 * io_splice_prep() - Menyiapkan operasi splice
 * @req: pointer ke struktur io_kiocb yang menyimpan permintaan I/O
 * @sqe: pointer ke struktur io_uring_sqe yang berisi parameter permintaan
 *
 * Fungsi ini mempersiapkan operasi splice, termasuk pengaturan offset untuk input dan output,
 * serta memanggil __io_splice_prep untuk validasi lebih lanjut.
 */

int io_splice_prep(struct io_kiocb *req, const struct io_uring_sqe *sqe)
{
	struct io_splice *sp = io_kiocb_to_cmd(req, struct io_splice);

	sp->off_in = READ_ONCE(sqe->splice_off_in);
	sp->off_out = READ_ONCE(sqe->off);
	return __io_splice_prep(req, sqe);
}

/*
 * io_splice() - Menjalankan operasi splice antara dua file
 * @req: pointer ke struktur io_kiocb yang menyimpan permintaan I/O
 * @issue_flags: flag yang digunakan untuk issue
 *
 * Fungsi ini melakukan operasi splice yang menyalin data dari file input ke file output
 * dengan mematuhi panjang yang ditentukan dan mengatur offset untuk input dan output.
 */
 
int io_splice(struct io_kiocb *req, unsigned int issue_flags)
{
	struct io_splice *sp = io_kiocb_to_cmd(req, struct io_splice);
	struct file *out = sp->file_out;
	unsigned int flags = sp->flags & ~SPLICE_F_FD_IN_FIXED;
	loff_t *poff_in, *poff_out;
	struct file *in;
	ssize_t ret = 0;

	WARN_ON_ONCE(issue_flags & IO_URING_F_NONBLOCK);

	in = io_splice_get_file(req, issue_flags);
	if (!in) {
		ret = -EBADF;
		goto done;
	}

	poff_in = (sp->off_in == -1) ? NULL : &sp->off_in;
	poff_out = (sp->off_out == -1) ? NULL : &sp->off_out;

	if (sp->len)
		ret = do_splice(in, poff_in, out, poff_out, sp->len, flags);

	if (!(sp->flags & SPLICE_F_FD_IN_FIXED))
		fput(in);
done:
	if (ret != sp->len)
		req_set_fail(req);
	io_req_set_res(req, ret, 0);
	return IOU_OK;
}
