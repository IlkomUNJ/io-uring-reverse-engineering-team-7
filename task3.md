# Task 3: Data Structure Investigation

NAMA : EVI SYIFA ROHMANIYAH
NIM : 1313622024

The objective of this task is to document all internal data structures defined in io_uring.

| Structure name | Defined in         | Attributes                            | Caller Functions Source   | source caller         | usage                                      |
|----------------|--------------------|----------------------------------------|----------------------------|------------------------|---------------------------------------------|
| io_ev_fd       | io_uring/eventfd.c     | eventfd_ctx, uint, uint, refcount_t, atomic_t, rcu_head | io_eventfd_free          | io_uring/eventfd.c  | local variable                        |
|                |                        |                                          | io_eventfd_put           |                      | function parameter                    |
|                |                        |                                          | io_eventfd_do_signal     |                      | local variable, function parameter    |
|                |                        |                                          |io_eventfd_signal      |                      | function parameter                    |
|                |                        |                                          | io_eventfd_grab          |                      | return value, local variable          |
|                |                        |                                          | io_eventfd_signal        |                      | local variable                        |
|                |                        |                                          | io_eventfd_flush_signal  |                      | local variable                        |
|                |                        |                                          | io_eventfd_register      |                      | local variable                        |
|                |                        |                                          | io_eventfd_unregister    |                      | function parameter                    |
| io_madvise     | io_uring/advise.c | file, addr, len, advice          | io_kiocb_to_cmd          | io_uring/advise.c | local variable       |
|                |                |                                    | io_kiocb_to_cmd          |               | local variable       |
| io_cancel      | io_uring/cancel.c | file, addr, flags, fd, opcode       | io_kiocb_to_cmd          | io_uring/cancel.c | local variable                   |
|                |                  |                                      | io_kiocb_to_cmd          |                  | local variable                   |
| io_cancel_data    | io_uring/cancel.h | ctx, data/file (union), opcode, flags, seq          | io_futex_cancel              | io_uring/futex.c   | function parameter         |
|                  |                  |                                                      | io_poll_cancel               | io_uring/poll.c    | function parameter         |
|                  |                  |                                                      | io_waitid_cancel             | io_uring/waitid.c  | function parameter         |
|                  |                  |                                                      | io_try_cancel                | io_uring/cancel.c  | function parameter         |
|                  |                  |                                                      | io_cancel_req_match          | io_uring/cancel.c  | function parameter         |
|                  |                  |                                                      | io_cancel_remove             | io_uring/cancel.c  | function parameter         |
|                  |                  |                                                      | io_timeout_cancel            | io_uring/timeout.c | function parameter         |
|                  |                  |                                                      | io_async_cancel            | io_uring/cancel.c  | function parameter         |
|                  |                  |                                                      |io_poll_cancel             | io_uring/poll.c    | function parameter         |
| io_epoll         | io_uring/epoll.c | file, epfd, op, fd, event                                                  | io_epoll_ctl_prep            | io_uring/epoll.c  | local variable             |
|                  |                |                                                                             | io_epoll_ctl                 | io_uring/epoll.c  | local variable             |
| io_epoll_wait    | io_uring/epoll.c | file, maxevents, events (user pointer to epoll_event)                      | io_epoll_wait_prep           | io_uring/epoll.c  | local variable             |
|                  |                |                                                                             | io_epoll_wait                | io_uring/epoll.c  | local variable             |
| io_rename      | fs.c      | file, old_dfd, new_dfd, oldpath, newpath, flags                        | io_renameat_prep              | fs.c             | local variable   |
|                |              |                                                                        | io_renameat                   | fs.c             | local variable   |
|                |              |                                                                        | io_renameat_cleanup           | fs.c             | local variable   |
|                |              |                                                                        | Used in `opdef.c`             | opdef.c          | .prep, .issue, .cleanup |
| io_unlink      | fs.c      | file, dfd, flags, filename                                             | io_unlinkat_prep              | fs.c             | local variable   |
|                |              |                                                                        | io_unlinkat                   | fs.c             | local variable   |
|                |              |                                                                        | io_unlinkat_cleanup           | fs.c             | local variable   |
|                |              |                                                                        | Used in `opdef.c`             | opdef.c          | .prep, .issue, .cleanup |
| io_mkdir       | fs.c     | file, dfd, mode, filename                                              | io_mkdirat_prep               | fs.c             | local variable   |
|                |              |                                                                        | io_mkdirat                    | fs.c             | local variable   |
|                |              |                                                                        | io_mkdirat_cleanup            | fs.c             | local variable   |
|                |              |                                                                        | Used in `opdef.c`             | opdef.c          | .prep, .issue, .cleanup |
| io_link        | fs.c     | file, old_dfd, new_dfd, oldpath, newpath, flags                        | io_linkat_prep                | fs.c             | local variable   |
|                |              |                                                                        | io_linkat                     | fs.c             | local variable   |
|                |              |                                                                        | io_link_cleanup               | fs.c             | local variable   |
|                |              |                                                                        | Used in `opdef.c`             | opdef.c          | .prep, .issue, .cleanup |
| io_futex       | futex.c    | file, uaddr/uwaitv (union), futex_val, futex_mask, futexv_owned, futex_flags, futex_nr, futexv_unqueued | - io_kiocb_to_cmd(req, struct io_futex)                    | futex.c     | local variable |
|                |            |                                                                                                | - io_futexv_claim(struct io_futex *iof)                     | futex.c     | parameter      |
| io_futex_data    | io_uring/futex.c  | futex_q q, io_kiocb *req                                         | io_futex_wake             | io_uring/futex.c       | local variable      |
| io_defer_entry     | io_uring/io_uring.c | list_head list, io_kiocb *req, u32 seq | io_req_defer               | io_uring/io_uring.c    | local variable      |
|                    |                    |                                   | io_req_task_work_add        | io_uring/io_uring.c    | local variable      |
| io_wait_queue     | io_uring/wait.c      | wait_queue_entry wq, io_ring_ctx *ctx, cq_tail, cq_min_tail, nr_timeouts, ... | io_commit_cqring         | io_uring/wait.c        | local variable      |
|                   |                      |                                                                              | io_flush_timeouts        | io_uring/wait.c        | local variable      |
| io_worker            | io_uring/wq.c      | ref, flags, nulls_node, task, wq, acct, cur_work | io_wq_worker_fn              | io_uring/wq.c          | local variable      |
| io_wq_acct           | io_uring/wq.c      | workers_lock, nr_workers, free_list, flags       | io_wq_create_worker          | io_uring/wq.c          | local variable      |
| io_wq                | io_uring/wq.c      | state, acct, worker_refs, cpu_mask               | io_wq_create                 | io_uring/wq.c          | local variable      |
| io_cb_cancel_data    | io_uring/wq.c      | fn, data, nr_running, nr_pending, cancel_all     | io_wq_cancel_cb              | io_uring/wq.c          | function parameter  |
| io_provide_buf     | io_uring/buffer.c      | file, addr, len, bgid, nbufs, bid          | io_provide_buffers           | io_uring/buffer.c       | function parameter  |
|                    |                        |                                            | io_provide_buffers_prep      | io_uring/buffer.c       | local variable      |
| io_buffer_list     | io_uring/buffer.c    | buf_list/buf_ring, bgid, buf_nr_pages, region   | io_buffer_select             | io_uring/buffer.c      | local variable      |
|                    |                      |                                                 | io_add_buffers               | io_uring/buffer.c      | local variable      |
| io_buffer          | io_uring/buffer.c    | list, addr, len, bid, bgid                      | io_cqring_fill_event         | io_uring/buffer.c      | local variable      |
| buf_sel_arg      | io_uring/kbuf.h    | iovs, out_len, max_len, nr_iovs, mode      | io_buffers_select          | io_uring/kbuf.c      | function parameter  |
|                  |                    |                                            | io_buffers_peek            | io_uring/kbuf.c      | function parameter  |
|                  |                    |                                            | (optional from net.c)      | io_uring/net.c       | local variable      |
| io_msg         | io_uring/msg_ring.c | file, src_file, tw, user_data, len, cmd, src_fd, dst_fd/cqe_flags, flags | io_msg_ring_prep           | io_uring/msg_ring.c    | function parameter  |
|                |                     |                                                                    | io_msg_ring                 | io_uring/msg_ring.c    | function parameter  |
|                |                     |                                                                    | __io_msg_ring_prep          | io_uring/msg_ring.c    | function parameter  |
|                |                     |                                                                    | io_msg_ring_data            | io_uring/msg_ring.c    | local variable      |
|                |                     |                                                                    | io_msg_grab_file            | io_uring/msg_ring.c    | local variable      |
|                |                     |                                                                    | io_msg_fd_remote            | io_uring/msg_ring.c    | local variable      |
|                |                     |                                                                    | io_msg_send_fd              | io_uring/msg_ring.c    | local variable      |
|                |                     |                                                                    | io_msg_install_complete     | io_uring/msg_ring.c    | local variable      |
|                |                     |                                                                    | io_msg_tw_fd_complete       | io_uring/msg_ring.c    | local variable      |
| io_napi_entry     | io_uring/napi.c  | napi_id, list, timeout, node, rcu                 | io_napi_hash_find           | io_uring/napi.c     | return value                 |
|                   |                  |                                                   | (multiple unnamed functions) | io_uring/napi.c     | local variable |
|
| io_msg         | io_uring/msg_ring.c | file, src_file, tw, user_data, len, cmd, src_fd, dst_fd/cqe_flags, flags | io_msg_ring_prep           | io_uring/msg_ring.c    | function parameter  |
|                |                   |                                                      | io_msg_ring                 | io_uring/msg_ring.c    | function parameter  |
|                |                   |                                                      | io_msg_ring_data            | io_uring/msg_ring.c    | local variable      |
|                |                   |                                                      | io_msg_send_fd              | io_uring/msg_ring.c    | local variable      |
|                |                   |                                                      | io_msg_grab_file            | io_uring/msg_ring.c    | local variable      |
|                |                   |                                                      | io_msg_fd_remote            | io_uring/msg_ring.c    | local variable      |
|                |                   |                                                      | io_msg_tw_fd_complete       | io_uring/msg_ring.c    | local variable      |
| io_napi_entry  | io_uring/napi.c    | napi_id, list, timeout, node, rcu                   | io_napi_hash_find           | io_uring/napi.c        | return value        |
|                |                   |                                                      | (multiple functions)        | io_uring/napi.c        | local variable       |
| io_shutdown    | io_uring/net.c     | file, how                                            | io_shutdown_prep            | io_uring/net.c         | function parameter   |
|                |                   |                                                      | io_shutdown                 | io_uring/net.c         | function parameter   |
| io_accept      | io_uring/net.c     | file, addr, addr_len, flags, iou_flags, file_slot, nofile | io_accept_prep             | io_uring/net.c         | function parameter   |
|                |                   |                                                      | io_accept                   | io_uring/net.c         | function parameter   |
| io_socket      | io_uring/net.c     | file, domain, type, protocol, flags, file_slot, nofile | io_socket_prep             | io_uring/net.c         | function parameter   |
|                |                   |                                                      | io_socket                   | io_uring/net.c         | function parameter   |
| io_connect     | io_uring/net.c     | file, addr, addr_len, in_progress, seen_econnaborted | io_connect_prep            | io_uring/net.c         | function parameter   |
|                |                   |                                                      | io_connect                  | io_uring/net.c         | function parameter   |
| io_bind        | io_uring/net.c     | file, addr_len                                      | io_bind_prep                | io_uring/net.c         | function parameter   |
|                |                   |                                                      | io_bind                     | io_uring/net.c         | function parameter   |
| io_listen      | io_uring/net.c     | file, backlog                                       | io_listen_prep              | io_uring/net.c         | function parameter   |
|                |                   |                                                      | io_listen                   | io_uring/net.c         | function parameter   |
| io_sr_msg      | io_uring/net.c     | file, umsg/umsg_compat/buf, len, msg_flags, flags, retry, notif, etc. | io_recvmsg_multishot       | io_uring/net.c         | function parameter   |
|                |                   |                                                      | (multiple other internal)   | io_uring/net.c         | local variable       |
| io_recvzc      | io_uring/net.c     | file, msg_flags, flags, len, ifq                    | io_recvzc_prep              | io_uring/net.c         | function parameter   |
|                |                   |                                                      | io_recvzc                   | io_uring/net.c         | function parameter   |
| io_nop         | io_uring/nop.c   | file, result, fd, flags                        | io_nop_prep              | io_uring/nop.c     | local variable      |
|                |                  |                                                | io_nop                   | io_uring/nop.c     | local variable      |
| io_notif_data     | io_uring/notif.h   | file, uarg, next, head, account_pages, zc_report, zc_used, zc_copied | io_notif_flush            | io_uring/notif.c     | local variable      |
|                   |                    |                                                      | io_notif_flush_last        | io_uring/notif.c     | local variable      |
|                   |                    |                                                      | io_notif_send              | io_uring/net.c       | local variable      |
| io_open              | io_uring/openclose.c | file, dfd, file_slot, filename, how, nofile                 | io_openat_prep               | io_uring/openclose.c   | local variable      |
|                      |                      |                                                             | io_openat2_prep              | io_uring/openclose.c   | local variable      |
|                      |                      |                                                             | io_openat2                   | io_uring/openclose.c   | local variable      |
|                      |                      |                                                             | io_openat                    | io_uring/openclose.c   | local variable      |
|                      |                      |                                                             | io_open_cleanup              | io_uring/openclose.c   | local variable      |
| io_close             | io_uring/openclose.c | file, fd, file_slot                                         | io_close_prep                | io_uring/openclose.c   | local variable      |
|                      |                      |                                                             | io_close                     | io_uring/openclose.c   | local variable      |
|                      |                      |                                                             | io_close_fixed               | io_uring/openclose.c   | local variable      |
| io_fixed_install     | io_uring/openclose.c | file, o_flags                                               | (anonymous use)              | io_uring/openclose.c   | local variable      |   
| io_poll_update     | io_uring/poll.c    | file, old_user_data, new_user_data, events, update_events, update_user_data | io_poll_update              | io_uring/poll.c        | local variable      |
|                    |                    |                                                                  | io_poll_update_prep         | io_uring/poll.c        | local variable      |
| io_poll_table      | io_uring/poll.c    | pt, req, nr_entries, error, owning, result_mask                 | __io_queue_proc             | io_uring/poll.c        | function parameter  |
|                    |                    |                                                                  | io_arm_poll_handler         | io_uring/poll.c        | local variable      |
| io_rsrc_update     | io_uring/rsrc.c  | file, arg, nr_args, offset             | io_rsrc_update           | io_uring/rsrc.c    | local variable   |
| io_splice      | io_uring/splice.c | file_out, off_out, off_in, len, splice_fd_in, flags, rsrc_node   | io_splice_prep               | io_uring/splice.c     | local variable      |
|                |                   |                                                                   | io_splice                    | io_uring/splice.c     | local variable      |
|                |                   |                                                                   | io_splice_cleanup            | io_uring/splice.c     | local variable      |
|                |                   |                                                                   | io_splice_get_file           | io_uring/splice.c     | local variable      |
| io_sq_data     | io_uring/sqpoll.h  | refs, park_pending, lock, ctx_list, thread, wait, sq_cpu, state, ...  | io_sq_thread_park              | io_uring/sqpoll.c      | function parameter  |
|                |                    |                                                                        | io_sq_thread_unpark            | io_uring/sqpoll.c      | function parameter  |
|                |                    |                                                                        | io_sq_thread_stop              | io_uring/sqpoll.c      | function parameter  |
|                |                    |                                                                        | io_put_sq_data                 | io_uring/sqpoll.c      | function parameter  |
|                |                    |                                                                        | io_uring_cancel_generic        | io_uring/io_uring.c    | function parameter  |
| io_statx       | io_uring/statx.c | file, dfd, mask, flags, filename, buffer                  | io_statx_prep                | io_uring/statx.c       | local variable      |
|                |                  |                                                            | io_statx                     | io_uring/statx.c       | local variable      |
|                |                  |                                                            | io_statx_cleanup             | io_uring/statx.c       | local variable      |
| io_sync        | io_uring/sync.c  | file, len, off, flags, mode                   | io_sync_file_range           | io_uring/sync.c        | local variable      |
io_tctx_node   | io_uring/tctx.h | ctx_node, task, ctx     | io_ring_exit_work         | io_uring/io_uring.c     | local variable
|              |                |                          | io_tctx_exit              | io_uring/io_uring.c     | local variable
|              |                |                          | io_ring_add_tctx_node     | io_uring/io_uring.c     | local variable, return value
|              |                |                          | io_get_task_ring          | io_uring/io_uring.c     | local variable
|              |                |                          | io_register_rsrc          | io_uring/register.c     | local variable
|              |                |                          | io_poll_task_func         | io_uring/cancel.c       | local variable
|              |                |                          | io_tctx_node_find         | io_uring/tctx.c         | local variable, return value
|              |                |                          | io_tctx_node_alloc        | io_uring/tctx.c         | return value
|              |                |                          | io_tctx_node_finish       | io_uring/tctx.c         | function parameter
|              |                |                          | io_tctx_node_ref          | io_uring/tctx.c         | function parameter
io_timeout     | io_uring/timeout.c | file, off, target_seq, repeats, list, head, prev | io_timeout_prep               | io_uring/timeout.c       | local variable
|              |                   |                                                  | io_timeout                    | io_uring/timeout.c       | local variable
|              |                   |                                                  | io_timeout_fn                 | io_uring/timeout.c       | local variable
|              |                   |                                                  | io_timeout_complete           | io_uring/timeout.c       | local variable
|              |                   |                                                  | io_timeout_extract            | io_uring/timeout.c       | return value, local variable
|              |                   |                                                  | io_timeout_cancel             | io_uring/timeout.c       | local variable
|              |                   |                                                  | io_timeout_prep             | io_uring/timeout.c       | local variable
|io_timeout_rem    | io_uring/timeout.c | file, addr, ts, flags, ltimeout       | io_timeout_remove_prep         | io_uring/timeout.c       | local variable
|                 |                   |                                        | io_timeout_remove              | io_uring/timeout.c       | local variable
|                 |                   |                                        | io_timeout_update              | io_uring/timeout.c       | function parameter (via `tr`)|
|io_timeout_data     | io_uring/timeout.h | req, timer, ts, mode, flags              | io_timeout_prep               | io_uring/opdef.c         | async_size
|                   |                    |                                           | io_timeout                    | io_uring/timeout.c       | req->async_data (local var)
|                   |                    |                                           | io_timeout_fn                 | io_uring/timeout.c       | container_of from hrtimer
|                   |                    |                                           | io_timeout_complete           | io_uring/timeout.c       | container_of from hrtimer
|                   |                    |                                           | io_timeout_prep             | io_uring/timeout.c       | hrtimer_setup parameter
|                   |                    |                                           | io_timeout_get_clock          | io_uring/timeout.c       | function parameter
|                   |                    |                                           | io_link_timeout_fn            | io_uring/timeout.c       | async_data from linked request|
|io_ftrunc         | io_uring/truncate.c| file, len              | io_ftruncate_prep            | io_uring/truncate.c      | local variable (cast from req)
|                 |                    |                        | io_ftruncate                 | io_uring/truncate.c      | local variable (cast from req)|
|io_async_cmd      | io_uring/uring_cmd.h | data, vec, sqes                        | io_uring_cmd_complete_inTask | io_uring/uring_cmd.c   | local variable (cast from req->async_data)
|                 |                      |                                         | io_uring_cmd_prep_async      | io_uring/uring_cmd.c   | req->async_data|
|io_waitid         | io_uring/waitid.c  | file, which, upid, options, refs, head, infop, info              | io_waitid_compat_copy_si     | io_uring/waitid.c      | function parameter|
|                 |                    |                                                                   | io_waitid_* (berbagai fungsi)| io_uring/waitid.c      | local variable (cast from req)|
|io_waitid_async      | io_uring/waitid.h  | req, wo (wait_opts)                    | io_waitid_async_* (berbagai fungsi) | io_uring/waitid.c      | function parameter, req->async_data|
|                    |                    |                                         |                               |                        | local variable (cast from req)|
|io_xattr        | io_uring/xattr.c | file, ctx (kernel_xattr_ctx), filename | io_xattr_* (berbagai fungsi) | io_uring/xattr.c    | function parameter, local variable (cast from req->async_data)|
|                |                  |                                      |                            |                      | req->async_data|
|io_zcrx_args      | io_uring/zcrx.c  | req, ifq, sock, nr_skbs                      | zcrx.c                   | zcrx.c             | local variable, external data (arg.data)|
|io_zcrx_area      | io_uring/zcrx.h  | nia, ifq, user_refs, is_mapped, area_id, pages, freelist, etc.     | zcrx.c                   | zcrx.c             | function parameters, local variables, field in other struct (ifq->area)|
io_zcrx_ifq       | io_uring/zcrx.h| ctx, area, rq_ring, rqes, rq_entries, dev, netdev, locks, etc.            | zcrx.c                   | |io_zcrx_unmap_area, io_zcrx_map_area, io_zcrx_recv, dll | function arguments, local vars, member of ctx|