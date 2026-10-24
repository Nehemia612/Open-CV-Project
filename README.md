# Open-CV-Project
KPP_OpenCv_Nehemia_Theofilus


Nama: Nehemia Theofilus
NRP: 5025261117

Analisis Singkat:
Project ini adalah suatu project computer vision yang dibuat menggunakan C++ dan OpenCv untuk mendeteksi jarak objek, koordinat objek, dan lebar objek pada beberapa objek tertentu.

Tools yang digunakan: ang 
1. Bahasa Pemrograman: C++
2. Library Pendukung: OpenCV
3. YAML (untuk menyimpan nilai-nilai kalibrasi kamera)

Kendala:
1. Memerlukan waktu yang cukup lama
2. Sempat mengganti objek karena harus menyesuaikan color range agar tidak bertabrakan
3. Adanya delay pada kamera

Penjelasan calib:
Pertama, program memetakan koordinat objek dunia nyata sebagai acuan ukuran dan posisi fisik. Koordinat ini nantinya dicocokkan dengan hasil tangkapan kamera saat proses kalibrasi dan disimpan ke dalam berkas kalibrasi.yaml. Setelah kamera aktif, setiap frame video yang masuk akan diubah ukurannya menjadi 640 x 480 piksel menggunakan cv::resize, lalu dikonversi ke ruang warna abu-abu (grayscale) melalui cv::cvtColor. Selanjutnya, program mendeteksi dan menandai titik-titik sudut pada papan chessboard. Pada tahap ini, pengguna dapat menekan tombol spasi (' ') untuk mengambil beberapa sampel gambar (snapshot) dari berbagai sudut pandang yang berbeda. Setelah jumlah snapshot dirasa cukup, pengguna cukup menekan tombol 'c' untuk memicu program agar mulai menghitung parameter kalibrasi kamera."

Penjelasan main:
Program membaca dan mengambil data dari file kalibrasi.yaml menggunakan fungsi cv::FileStorage. Setelah itu, program menginisialisasi aliran video dan memasukkan setiap gambar yang tertampil ke dalam variabel matriks cv::Mat frame.Setiap frame yang masuk akan diubah ukurannya menjadi 640x480 piksel menggunakan cv::resize, lalu dikonversi dari ruang warna BGR ke HLS menggunakan cv::cvtColor agar lebih tahan terhadap perubahan pencahayaan. Selanjutnya, program melakukan color masking menggunakan fungsi cv::inRange berdasarkan rentang warna HLS yang telah ditentukan untuk masing-masing objek.Untuk membersihkan gangguan (noise) pada mask, program menerapkan operasi morfologi menggunakan cv::erode dan cv::dilate. Setelah itu, program mencari kontur objek menggunakan cv::findContours dan mendeteksi area objek menggunakan cv::boundingRect untuk membentuk kotak pembatas (bounding box).Berdasarkan data dari kotak pembatas tersebut, program menghitung koordinat spasial dan jarak objek menggunakan parameter intrinsik kamera yang sudah dimuat sebelumnya. Terakhir, program menghitung frame rate (FPS) menggunakan cv::getTickCount() dan merender seluruh informasi ke layar menggunakan cv::putText, cv::circle, serta menampilkannya secara real-time melalui cv::imshow.


