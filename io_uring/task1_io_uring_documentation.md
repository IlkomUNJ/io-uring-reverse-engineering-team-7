IO_Uring DOCUMENTATION TEAM 7

## Source
1. advise.c
Menangani dua jenis operasi: io_madvice dan io_fadvice. Keduanya menggunakan struktur data serupa (berisi file pointer, offset, dan panjang data), tetapi diproses oleh fungsi kernel yang berbeda. io_madvice hanya diaktifkan jika flag build tertentu (CONFIG_ADVISE_SYSCALLS) diaktifkan dan menggunakan do_madvise untuk memberi saran ke kernel tentang pengelolaan memori. Sementara io_fadvice aktif secara default dan menggunakan vfs_fadvise untuk menyarankan strategi caching file. Meski struktur dan logikanya hampir identik, keduanya diarahkan untuk konteks yang berbeda: memori vs file.

2. alloc_cache.c
File ini berfungsi mengatur cache alokasi fixed file table dalam io_uring. File ini menggunakan struktur fixed_file_ref_node dan fixed_file_table untuk mengelola referensi file yang digunakan secara berulang agar lebih efisien. Fungsi-fungsi di dalamnya menangani peminjaman, pengembalian, dan manajemen memori untuk tabel file yang dipetakan secara tetap. Tujuannya adalah mengurangi overhead alokasi file saat pengguna menggunakan fitur fixed files.

3. cancel.c
Mengatur pembatalan request yang diajukan ke io_uring, baik satu per satu maupun secara massal. Menyediakan implementasi pembatalan berdasarkan user_data, file descriptor, tag, dan bahkan kondisi internal kernel. Fungsi io_async_cancel dan io_cancel menjadi inti pembatalan request. File ini penting untuk menjamin bahwa request yang tidak dibutuhkan lagi bisa dihentikan dan sumber dayanya dibebaskan tanpa menunggu eksekusi selesai.

4. epoll.c
Membantu mengelola dan memanipulasi file descriptor dalam epoll untuk I/O non-blok. File ini menyimpan dua struktur utama: io_epoll dan io_epoll_wait. Keduanya menangani operasi berbasis epoll pada io_uring, tetapi dengan tujuan berbeda. io_epoll digunakan untuk operasi kontrol (epoll_ctl) seperti menambahkan, menghapus, atau memodifikasi event di epoll, sedangkan io_epoll_wait menangani event yang siap dikonsumsi melalui epoll_wait. Fungsi io_epoll_ctl_prep dan io_epoll_ctl mempersiapkan dan menjalankan kontrol epoll. Sebaliknya, io_epoll_wait_prep dan io_epoll_wait menangani proses menunggu event. Perbedaan implementasi mencerminkan perbedaan antara pengaturan epoll dan konsumsi event-nya.

5. eventfd.c
Menangani integrasi eventfd dalam io_uring, agar aplikasi bisa diberi sinyal saat ada hasil I/O baru di Completion Queue (CQ). Struktur io_ev_fd menyimpan eventfd dan status terkait. Fungsi io_eventfd_register mendaftarkan eventfd ke konteks io_uring, sedangkan io_eventfd_signal dan io_eventfd_flush_signal mengirim sinyal jika CQ diperbarui. Penggunaan refcount dan RCU memastikan akses data tetap aman dan memori dibebaskan dengan benar. File ini membantu sistem memberi notifikasi tanpa perlu polling terus-menerus.

6. fdinfo.c
Bertujuan untuk menampilkan informasi tentang status file descriptor dalam io_uring. Kode ini mengakses antrian permintaan (SQE) dan hasil (CQE) untuk memeriksa posisi head dan tail, serta mask yang digunakan untuk proses komunikasi. Fitur penting lainnya adalah polling NAPI, yang memungkinkan sistem untuk memantau status dan buffer yang digunakan tanpa membutuhkan polling terus-menerus. Fungsi-fungsi dalam file ini, seperti penguncian dan RCU, memastikan pengelolaan data dan memori tetap aman meski ada akses bersamaan.

7. filetable.c
File ini menangani manajemen tabel file dalam io_uring dengan fokus pada alokasi dan pembebasan file yang terkait dengan antrian I/O. Fungsi io_file_bitmap_get mencari slot kosong di bitmap untuk mengalokasikan file baru, sedangkan io_alloc_file_tables dan io_free_file_tables mengelola alokasi dan pembebasan tabel file. Fungsi io_install_fixed_file bertanggung jawab untuk menambahkan file tetap ke dalam tabel file yang sudah ada, memastikan alokasi slot yang valid. Fungsi io_fixed_fd_install dan io_fixed_fd_remove memungkinkan pemasangan dan penghapusan file tetap, mengelola referensi dan status slot. 

8. fs.c
Menyimpan beberapa struktur (io_rename, io_unlink, io_mkdir, io_link) untuk menjalankan operasi file system umum seperti renameat, unlinkat/rmdir, mkdirat, symlinkat, dan linkat secara asinkron melalui io_uring. Perbedaan utama terletak pada jenis operasi file system yang diwrap, tetapi pola umum adalah: baca input pengguna, lakukan syscall yang sesuai, lalu bersihkan resource. File ini menjembatani fitur file system tradisional dengan mekanisme asinkron io_uring.

9. futex.c
Memperluas dukungan futex ke model I/O asinkron io_uring dan memungkinkan operasi futex_wait dan futex_wake berjalan tanpa memblokir thread pengguna. Struktur io_futex menyimpan pointer ke alamat futex pengguna, nilai dan mask yang dipakai, serta flags operasi. Saat menyiapkan request (io_futex_prep), field ini dibaca langsung dari SQE. Penggunaan cache (alloc_cache) untuk data futex mempercepat alokasi internal, dan task-work digunakan untuk menyinkronkan pengiriman hasil kembali ke pengguna. 

10. io_uring.c
bertujuan mempercepat I/O dengan mengurangi peralihan antara ruang pengguna dan ruang kernel. Fungsi-fungsi utama dalam file ini seperti io_req_post_cqe, io_req_complete_post, dan io_req_defer_failed berfokus pada pengelolaan request I/O, termasuk menyelesaikan atau menunda operasi berdasarkan kondisi tertentu. 

11. io_wq.c
kode berfokus pada pengelolaan thread worker untuk io_uring, yang digunakan dalam pemrosesan I/O asinkron di kernel Linux. Ada beberapa struktur penting, yaitu io_worker yang merepresentasikan pekerja, dan io_wq yang menyimpan antrian pekerjaan. Fungsi utama dari kode ini adalah untuk membuat thread pekerja, mengatur pekerjaan yang akan dijalankan oleh pekerja tersebut, dan menghentikan pekerja setelah selesai.

12. kbuf.c
File kbuf.c mengimplementasikan manajemen buffer dalam sistem IO-uring pada kernel Linux. Di dalamnya, fungsi-fungsi seperti io_kbuf_commit berperan dalam melakukan komitmen data dari buffer ke ring buffer. Fungsi io_kbuf_alloc digunakan untuk mengalokasikan buffer yang diperlukan oleh aplikasi, sementara io_ring_buffer_select bertugas untuk memilih buffer yang siap digunakan dalam proses I/O. Setiap buffer yang dialokasikan dibedakan dengan ID unik yang memudahkan pengelolaan dan identifikasi. 

13. memmap.c
Kode ini mengelola memori untuk io_uring, sebuah antarmuka I/O asinkron di Linux, dengan cara mengalokasikan, memetakan, dan membebaskan wilayah memori yang digunakan dalam operasi I/O. Fungsi-fungsi utama seperti io_mem_alloc_compound menangani alokasi halaman memori besar, sementara io_pin_pages memastikan halaman tetap berada di tempatnya untuk operasi I/O. 

14. msg_ring.c
Ada beberapa mekanisme kunci untuk pengelolaan sinkronisasi, termasuk io_lock_external_ctx, yang memastikan penguncian yang tepat antara dua konteks, dan io_double_unlock_ctx, yang mengelola pembebasan kunci setelah operasi selesai. Untuk mengirim file descriptor, fungsi io_msg_send_fd bertanggung jawab untuk memverifikasi file sumber dan memastikan bahwa konteks pengirim dan penerima tidak sama.

15. napi.c
 Proses inisialisasi dan pembersihan diatur dalam file ini melalui fungsi io_napi_init yang menyiapkan struktur NAPI dalam konteks io_ring_ctx dan io_napi_free yang menghapus sumber daya yang tidak lagi diperlukan. Secara keseluruhan, file ini memainkan peran penting dalam mengelola polling dan sinkronisasi yang efisien dalam sistem IO-uring.

16. net.c
Bertanggung jawab untuk menangani operasi jaringan asinkron menggunakan io_uring dalam kernel Linux. Secara teknis, file ini mengelola berbagai jenis operasi jaringan, seperti penerimaan koneksi (io_accept), pengiriman dan penerimaan data (io_sendmsg, io_recvmsg), serta pengaturan socket (io_shutdown, io_connect, io_bind, io_listen).

17. nop.c
FIle menangani operasi no-op (no operation) dalam konteks io_uring di kernel Linux. Operasi no-op digunakan untuk keperluan kontrol aliran atau pengujian tanpa melakukan operasi I/O yang sesungguhnya. Secara teknis, file ini mengimplementasikan dua fungsi utama: io_nop_prep dan io_nop. Fungsi io_nop_prep mempersiapkan operasi no-op dengan membaca parameter yang dikirimkan dalam queue entry (sqe).

18. notif.c
File notif.c bertanggung jawab untuk mengelola notifikasi dalam sistem io_uring, khususnya terkait dengan pengelolaan buffer dan penyelesaian tugas notifikasi. Terdapat fungsi io_notif_tw_complete bertugas untuk menyelesaikan tugas notifikasi dengan memproses setiap elemen dalam daftar notifikasi yang terkait, sambil mengelola penggunaan memori yang terkait dengan notifikasi tersebut. 

19. opdef.c
File opdef.c adalah bagian dari implementasi pengelolaan opcode (operation code) untuk IO-uring, sebuah antarmuka yang digunakan untuk menangani operasi input/output (I/O). Setiap operasi dalam tabel io_issue_defs memiliki struktur yang mengarahkan ke fungsi prep yang menyiapkan data atau kondisi yang diperlukan sebelum operasi I/O, serta fungsi issue yang menjalankan operasi tersebut. Beberapa operasi juga memiliki mekanisme untuk menangani pembatalan atau timeout, serta penggunaan sumber daya khusus seperti buffer atau koneksi jaringan.

20. openclose.c
File ini mengelola berbagai operasi terkait file, seperti membuka file (open), menutup file (close), dan menginstal file tetap (fixed file).

21. poll.c
Secara teknis, file ini mengimplementasikan struktur data dan fungsi yang berhubungan dengan polling, termasuk struktur io_poll yang menyimpan informasi polling untuk sebuah request I/O, dan io_poll_update yang menangani pembaruan informasi terkait file yang dipantau. Beberapa fungsi utama dalam file ini meliputi pengelolaan referensi polling, seperti io_poll_get_ownership yang menangani mekanisme pengambilan kepemilikan untuk polling, dan io_poll_check_events yang memeriksa apakah suatu polling telah selesai atau perlu diulang.

22. register.c
File ini mengimplementasikan mekanisme registrasi dan pembatalan berbagai aspek dalam konteks io_uring, termasuk pendaftaran dan penghapusan personalitas, pengaturan batasan, serta pengaturan afinitas CPU untuk antrian tugas. 

23. rsrc.c
File berisi implementasi syscall io_uring_register() yang digunakan untuk mendaftarkan atau mencabut berbagai jenis sumber daya atau konfigurasi tambahan pada sebuah konteks io_uring  termasuk pendaftaran operation restriction, personality (identitas keamanan proses), afinitas CPU untuk thread worker (iowq), batas maksimal worker, aktivasi cincin (enable_rings), dan probing dukungan operasi.

24. rw.c
Beberapa fungsi kunci didefinisikan untuk menangani buffer dan iterasi I/O seperti io_import_rw_buffer(), io_rw_alloc_async(), dan io_req_rw_cleanup(). Selain itu, disediakan mekanisme penanganan metadata melalui struktur io_uring_attr_pi, serta penanganan untuk buffer yang dipilih secara otomatis (buffer select). Intinya  berisi implementasi logika untuk operasi read/write (I/O sinkron dan asinkron).

25. splice.c
Berisi tentang mekanisme pemindahan data antar file descriptor tanpa perlu memindahkan data ke userspace terlebih dahulu. Secara teknis, file ini mendefinisikan struktur io_splice untuk menyimpan parameter operasi splice seperti file sumber, tujuan, offset, panjang data, dan flag. Fungsi-fungsi utama seperti io_splice_prep() dan io_tee_prep() bertugas untuk mempersiapkan request berdasarkan data dari submission queue entry (SQE), termasuk validasi flag dan pengambilan file descriptor.

26. sqpoll.c
Secara teknis, file ini mendefinisikan logika untuk mengelola thread SQPOLL melalui struktur io_sq_data. Fungsi seperti io_sq_thread_park, io_sq_thread_unpark, dan io_sq_thread_stop mengatur status dan siklus hidup thread tersebut, termasuk parkir saat tidak aktif, dan membangunkannya saat ada permintaan I/O baru. Fungsi __io_sq_thread menjalankan loop pemrosesan I/O yang memanggil io_submit_sqes untuk memproses submission queue entries jika ada, serta melakukan polling terhadap antrean I/O yang memerlukan polling (misalnya untuk non-blocking I/O).

27. statx.c
Fungsi io_statx sendiri melakukan pemanggilan inti do_statx, yang merupakan fungsi kernel untuk mendapatkan metadata file, lalu menyimpan hasilnya ke dalam struktur yang disediakan oleh pengguna. Setelah eksekusi, hasil disimpan melalui io_req_set_res. Sementara itu, io_statx_cleanup bertugas membersihkan sumber daya yang digunakan seperti filename, dengan memanggil putname jika diperlukan.

28. sync.c
Seluruh operasi ini diatur untuk memaksa eksekusi secara asynchronous dengan menandai permintaan menggunakan REQ_F_FORCE_ASYNC, tetapi tetap dilakukan dalam konteks blocking sesuai syarat sistem file kernel, yang dipastikan melalui WARN_ON_ONCE(issue_flags & IO_URING_F_NONBLOCK). Eksekusinya dilakukan pada io_sync_file_range, yang memanggil fungsi kernel sync_file_range untuk menyinkronkan sebagian file ke disk.

29. tctx.c
File berisi tentang konteks tugas (task context) dalam kerangka kerja io_uring di kernel Linux. File ini menyediakan berbagai fungsi untuk menangani operasi I/O asinkron, mengelola sumber daya terkait dengan tugas (task) yang sedang berjalan, serta mengelola file deskriptor yang terdaftar untuk digunakan dalam operasi io_uring..  Fungsi  __io_uring_add_tctx_node mengelola penambahan node konteks tugas ke dalam daftar konteks yang terkait dengan io_uring. Fungsi ini memastikan bahwa setiap tugas yang berinteraksi dengan io_uring memiliki entri yang terhubung dengan benar dalam daftar tugas tersebut. Jika konteks tugas belum ada, maka fungsi ini akan membuatnya dan menyimpannya di dalam struktur xarray. 

30. timeout.c
File ini mengimplementasikan berbagai mekanisme untuk menunda atau membatalkan operasi I/O dengan menggunakan timer berbasis hrtimer. Struktur utamanya adalah io_timeout, yang menghubungkan file dan informasi terkait timeout, seperti durasi timeout, urutan target, dan pengulangan. Struktur ini juga mengelola hubungan antara permintaan I/O yang memiliki timeout dan tugas terkait, baik untuk timeout biasa maupun yang terhubung dengan permintaan lainnya (linked timeouts).

31. truncate.c
File ini mendefinisikan struktur data dan fungsi yang menangani permintaan pemotongan file secara asinkron. Fungsi io_ftruncate bertugas untuk menjalankan operasi pemotongan file itu sendiri. Fungsi ini memeriksa apakah permintaan harus dilakukan secara non-blocking (yang tidak diperbolehkan dalam hal ini). Setelah itu, ia memanggil do_ftruncate untuk melakukan pemotongan file pada file yang ditentukan dengan panjang baru (len). Setelah operasi selesai, hasilnya akan diset pada permintaan tersebut melalui io_req_set_res.

32. uring_cmd.c
Di dalam file ini, berbagai fungsi digunakan untuk mempersiapkan, mengirim, dan membersihkan perintah-perintah tersebut, dengan fokus pada perintah terkait soket dan alokasi buffer yang digunakan dalam operasi tersebut. Selain itu, terdapat pengelolaan opsi soket seperti getsockopt dan setsockopt dalam bagian kode yang mengelola operasi pada soket jaringan. Hal ini memberikan dukungan untuk mengakses atau mengubah pengaturan soket.

33. waitid.c
ungsi utama dari file ini adalah menangani operasi terkait penanganan sinyal proses anak, serta memungkinkan penggunaan fitur I/O asinkron dengan io_uring. ile ini mengelola permintaan asinkron untuk fungsi waitid, yang memungkinkan aplikasi untuk menunggu proses anak tanpa memblokir eksekusi utama. Struktur data seperti io_waitid digunakan untuk menyimpan informasi terkait permintaan ini, termasuk informasi tentang proses yang sedang ditunggu, statusnya, dan sinyal yang akan diteruskan ke pengguna. Permintaan ini juga dapat dibatalkan jika terjadi kondisi tertentu, dengan penggunaan mekanisme penanganan referensi dan pengelolaan antrian tunggu.

34. xattr.c
File ini mengimplementasikan beberapa fungsi yang mengelola struktur data yang disebut io_xattr, yang berisi informasi tentang file yang sedang diproses, konteks xattr, dan nama file. Fungsi utama yang ada dalam file ini termasuk io_fgetxattr_prep dan io_getxattr_prep yang bertugas menyiapkan permintaan untuk mengambil atribut eksternal dari file, baik berdasarkan file descriptor (fgetxattr) maupun path (getxattr). Kedua fungsi ini memverifikasi dan menginisialisasi nama atribut dan ukuran data yang akan diambil. Fungsi io_fgetxattr dan io_getxattr kemudian memanggil fungsi untuk mendapatkan nilai xattr yang sesuai dari file atau sistem file.

35. zcrx.c
Mencakup beberapa fungsi utama yang terkait dengan pengelolaan memori untuk penerimaan data jaringan pada perangkat yang terhubung ke io_uring. Fungsi io_zcrx_map_area dan io_zcrx_unmap_area mengelola pemetaan dan pembebasan area memori yang digunakan untuk buffer data yang diterima, dengan memanfaatkan Direct Memory Access (DMA) untuk meminimalkan interaksi CPU dalam proses pemindahan data.

## Headers
1. advise.h
File ini hanya berisi deklarasi fungsi (function prototypes) tanpa implementasinya, yaitu io_madvise_prep, io_madvise, io_fadvise_prep, dan io_fadvise.

2. filetable.h
File mendukung infrastruktur internal io_uring untuk penggunaan file descriptor yang efisien, terutama saat menggunakan fitur fixed files demi menghindari overhead pengambilan file descriptor dari userspace saat melakukan I/O.

3. memmap.h
File ini menyediakan deklarasi fungsi dan makro yang digunakan dalam proses pemetaan region memory user-space ke kernel melalui mekanisme mmap, serta pengelolaan region tersebut. Beberapa fungsi kunci yang dideklarasikan di sini antara lain io_uring_get_unmapped_area, io_uring_mmap, io_create_region, dan io_create_region_mmap_safe, yang merupakan inti dari bagaimana io_uring mengalokasikan dan memetakan buffer yang digunakan untuk zero-copy atau pinned memory.

4. opdef.h
opdef.h bertanggung jawab mendefinisikan struktur dan deklarasi fungsi yang berkaitan dengan spesifikasi operasional dari setiap opcode (io_uring) yang didukung oleh kernel. Secara teknis, file ini mendeskripsikan dua struktur utama: io_issue_def dan io_cold_def. Struktur io_issue_def menyimpan properti dan perilaku dari operasi I/O yang akan dijalankan, seperti apakah operasi membutuhkan file descriptor, apakah mendukung iopoll, apakah vektor buffer digunakan, dan lainnya. 

5. slist.h
File ini bertanggung jawab menyediakan fungsi-fungsi manipulasi linked list satu arah (singly linked list) yang digunakan oleh infrastruktur io_uring untuk mengatur antrian pekerjaan. 

6. truncate.h
File hanya mendeklarasikan fungsi tanpa mendefinisikan implementasinya dan berisi deklarasi prototipe fungsi untuk operasi ftruncate dalam konteks io_uring. 

7. alloc_cache.h
File ini hanya berisi definisi dan implementasi fungsi inline, tanpa menyimpan data riil, dan akan digunakan oleh file .c terkait yang membutuhkan cache alokasi cepat dalam io_uring.

8. fs.h
Berisi fungsi-fungsi yang berkaitan dengan operasi file system non-IO dasar dalam konteks io_uring, seperti renameat, unlinkat, mkdirat, symlinkat, dan linkat. Setiap operasi memiliki tiga fungsi utama: prep, execute, dan cleanup. 

9. openclose.h
Hanya mendeklarasikan fungsi-fungsi yang berkaitan dengan operasi pembukaan dan penutupan file descriptor dalam konteks io_uring. Fungsinya terbagi menjadi dua kelompok utama: open dan close

10. splice.h
Hanya mendeklarasikan fungsi-fungsi yang berkaitan dengan operasi splice dan tee dalam konteks io_uring

11. uring_cmd.h
Hanya mendefinisikan struktur data dan deklarasi fungsi yang berhubungan dengan pengelolaan perintah dalam konteks IO uring, seperti io_async_cmd.

12. futex.h
Selain mendeklarasikan beberapa fungsi terkait operasi futex dalam konteks penggunaan io_uring, ile ini juga menyediakan implementasi yang berbeda tergantung pada apakah FUTEX diaktifkan dalam konfigurasi kernel (CONFIG_FUTEX). 

13. msg_ring.h
Hanya berisi fungsi-fungsi yang mengelola operasi message ring dalam konteks io_uring. 

14. poll.h
File ini mendefinisikan struktur dan fungsi yang berkaitan dengan polling asinkron, di mana polling digunakan untuk memeriksa status file atau perangkat untuk operasi I/O yang siap dijalankan. Ada dua struktur utama yang didefinisikan dalam file ini, yaitu io_poll dan async_poll.

15. sqpoll.h
Isinya mengenai mengelola data yang diperlukan untuk menjalankan thread polling yang menangani I/O operasi dalam io_uring. Fungsi-fungsi yang ada dalam file ini mengelola siklus hidup thread, seperti memulai, menghentikan, dan mengatur thread yang bertanggung jawab untuk melakukan polling terhadap SQ.

16. waitid.h
File ertanggung jawab dalam mendefinisikan struktur data dan fungsi yang terkait dengan penanganan operasi asinkron dalam sistem I/O berbasis io_uring. Secara teknis, file ini mengimpor exit.h dari kernel untuk mendukung pengelolaan proses atau tugas yang terkait dengan I/O. Struktur data io_waitid_async menyimpan informasi yang berkaitan dengan permintaan I/O (req) dan opsi tunggu (wo) yang digunakan dalam operasi penantian asinkron.

17. io_uring.h
File header io_uring.h di kernel Linux mendefinisikan berbagai struktur dan makro untuk mendukung implementasi io_uring, yang memungkinkan pengolahan I/O secara asinkron dan efisien. Struktur utama yang ada dalam file ini meliputi io_uring, yang menyimpan informasi tentang antrian permintaan I/O, serta dua antrian utama yaitu submit queue (SQ) dan completion queue (CQ). Struktur io_sqe (Submit Queue Entry) berfungsi untuk mendeskripsikan setiap operasi I/O yang dikirimkan ke io_uring, sedangkan struktur io_cqe (Completion Queue Entry) digunakan untuk menggambarkan hasil dari operasi I/O yang telah selesai.

18. napi.h
ile header napi.h di kernel Linux berfungsi untuk mendukung integrasi busy poll dengan io_uring, memungkinkan pengolahan I/O yang lebih efisien dalam konteks jaringan. Fitur ini bergantung pada konfigurasi CONFIG_NET_RX_BUSY_POLL yang mengaktifkan kemampuan polling pada socket menggunakan NAPI (New API) untuk meningkatkan kinerja saat menerima data dalam mode polling.

19. refs.h
Filenya mengelola referensi (counter) untuk objek io_kiocb di dalam sistem io_uring. Fungsi-fungsi dalam file ini bekerja dengan atomic operations untuk memastikan bahwa perubahan pada counter referensi dilakukan secara aman dalam lingkungan yang multithreaded.

20. statx.h
Berisi ungsi-fungsi yang didefinisikan di dalamnya mengelola persiapan, eksekusi, dan pembersihan untuk operasi statx, yang digunakan untuk memperoleh status file atau objek dalam sistem file.

21. xattr.h
Hanya berisi deklarasi fungsi untuk menangani operasi terkait dengan extended attributes (xattr) pada file atau objek dalam sistem file menggunakan io_uring. 

22. io-wq.h
File io-wq.h mendefinisikan struktur dan fungsi yang terkait dengan pengelolaan work queue (wq) dalam konteks io_uring, yang digunakan untuk menjalankan pekerjaan secara asinkron dengan memanfaatkan thread terpisah atau terdistribusi, terutama dalam pemrosesan I/O. Bagian pertama dari file ini mendefinisikan sejumlah enum dan konstanta untuk mengatur status pekerjaan dalam work queue, seperti IO_WQ_WORK_CANCEL untuk membatalkan pekerjaan, IO_WQ_WORK_HASHED untuk menandakan pekerjaan yang ter-hash, dan IO_WQ_WORK_CONCURRENT untuk pekerjaan yang bisa berjalan bersamaan. IO_WQ_HASH_SHIFT digunakan untuk pengalamatan hash key dalam pekerjaan.

23. net.h
File net.h berfokus pada pengelolaan operasi jaringan asinkron dalam konteks io_uring, yang memungkinkan operasi I/O pada socket untuk dilakukan secara efisien menggunakan antarmuka berbasis submission queue dan completion queue. Di dalam file ini, terdapat struktur dan fungsi yang mendukung pengiriman dan penerimaan pesan melalui socket, serta operasi lainnya seperti bind, listen, dan accept.

24. register.h
Hanya erisi deklarasi fungsi-fungsi yang terkait dengan pendaftaran dan pengelolaan objek-objek yang digunakan oleh io_uring

25. sync.h
Hanya mendefinisikan fungsi-fungsi yang berkaitan dengan operasi sinkronisasi berkas dan alokasi ruang berkas dalam konteks io_uring.

26. zcrx.h
File zcrx.h berfungsi untuk mendefinisikan dan mengelola mekanisme Zero Copy Receive (ZCRX) dalam io_uring. Secara teknis, file ini mendefinisikan struktur data dan fungsi-fungsi terkait yaitu framework asynchronous I/O di Linux. 

27. kbuf.h
Berisi tentang pengaturan manajemen buffer dalam konteks io_uring untuk menangani operasi I/O buffer secara lebih efisien. Beberapa struktur penting yang terdapat dalam file ini adalah io_buffer_list yang menyimpan daftar buffer dan informasi terkait, serta io_buffer yang mewakili buffer individual yang akan diproses. 

28. nop.h
Hanya endeklarasikan dua fungsi untuk operasi "no-op" (no operation) pada io_uring. 

29. rsrc.h
File rsrc.h mengelola sumber daya dalam konteks io_uring di Linux, yang berfokus pada alokasi, pembebasan, dan pengelolaan buffer dan file dalam operasi I/O asinkron. 

30. tctx.h
File tctx.h berkaitan dengan pengelolaan konteks tugas (task context). Berisi Fungsi utama dalam file ini adalah io_uring_alloc_task_context, yang mengalokasikan konteks untuk sebuah tugas, serta __io_uring_add_tctx_node dan __io_uring_add_tctx_node_from_submit, yang menambah node konteks tugas ke dalam daftar terkait. 

31. fdinfo.h
Hanya berisi deklarasi fungsi io_uring_show_fdinfo, yang bertanggung jawab untuk menampilkan informasi terkait file descriptor yang digunakan dalam konteks io_uring. 
