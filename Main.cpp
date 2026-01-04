#include <iostream>
#include <iomanip>
#include <limits>
#include <cstring>
#include <ctime>

// Include semua file header modular
#include "user.h"
#include "data_structures.h"
#include "file_handler.h"
#include "admin.h"

using namespace std;

// =================================================================
// STRUKTUR DATA GLOBAL
// =================================================================
// Struktur data ini diakses oleh Admin dan User
RefundStack globalRefundStack;        // Stack untuk refund (LIFO)
CSQueue globalCSQueue;                // Queue untuk CS (FIFO)
AVLTree globalAVLTree;                // AVL Tree untuk transaksi

// =================================================================
// FUNGSI UTILITY
// =================================================================
void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void pause() {
    cout << "\nTekan ENTER untuk melanjutkan...";
    cin.get();
}

void generateTimestamp(char* ts) {
    time_t now = time(0);
    struct tm* t = localtime(&now);
    sprintf(ts, "%02d/%02d/%04d %02d:%02d:%02d",
            t->tm_mday, t->tm_mon + 1, t->tm_year + 1900,
            t->tm_hour, t->tm_min, t->tm_sec);
}

// =================================================================
// FUNGSI VALIDASI INPUT
// =================================================================
bool isValidNIK(const char* nik) {
    if (strlen(nik) != 16) return false;
    for (int i = 0; i < 16; i++) {
        if (nik[i] < '0' || nik[i] > '9') return false;
    }
    return true;
}

bool isValidPIN(const char* pin) {
    if (strlen(pin) != 6) return false;
    for (int i = 0; i < 6; i++) {
        if (pin[i] < '0' || pin[i] > '9') return false;
    }
    return true;
}

bool isValidDate(const char* date) {
    // Format: DD/MM/YYYY
    if (strlen(date) != 10) return false;
    if (date[2] != '/' || date[5] != '/') return false;
    
    for (int i = 0; i < 10; i++) {
        if (i != 2 && i != 5) {
            if (date[i] < '0' || date[i] > '9') return false;
        }
    }
    
    // Validasi range tanggal
    int day = (date[0] - '0') * 10 + (date[1] - '0');
    int month = (date[3] - '0') * 10 + (date[4] - '0');
    int year = (date[6] - '0') * 1000 + (date[7] - '0') * 100 + 
               (date[8] - '0') * 10 + (date[9] - '0');
    
    if (day < 1 || day > 31) return false;
    if (month < 1 || month > 12) return false;
    if (year < 1900 || year > 2024) return false;
    
    return true;
}

bool isValidUsername(const char* username) {
    int len = strlen(username);
    if (len < 3 || len > 49) return false;
    
    // Username tidak boleh ada spasi
    for (int i = 0; i < len; i++) {
        if (username[i] == ' ') return false;
    }
    return true;
}

bool isValidPassword(const char* password) {
    return strlen(password) >= 6;
}

// =================================================================
// DEKLARASI FUNGSI (Forward Declaration)
// =================================================================
void demoHuffman();
void userMenu(User* user, User* users, int count, FileHandler& fileHandler);

// =================================================================
// FUNGSI REGISTRASI USER BARU DENGAN DATA KTP
// =================================================================
bool registerUser(User*& users, int& userCount, FileHandler& fileHandler) {
    clearScreen();
    cout << "\n";
    cout << "========================================\n";
    cout << "   REGISTRASI USER BARU\n";
    cout << "   E-WALLET SYSTEM\n";
    cout << "========================================\n";
    cout << "\nSilakan lengkapi data berikut untuk membuat akun baru.\n";
    cout << "Pastikan data KTP Anda valid dan benar.\n";
    
    char username[50], password[50], passwordConfirm[50], pin[7], pinConfirm[7];
    KTP ktp;
    
    // =================================================================
    // STEP 1: Input Username
    // =================================================================
    cout << "\n=== STEP 1: DATA LOGIN ===\n";
    cout << "\nUsername (3-49 karakter, tanpa spasi): ";
    cin.getline(username, 50);
    
    if (!isValidUsername(username)) {
        cout << "\n[ERROR] Username tidak valid!\n";
        cout << "- Minimal 3 karakter, maksimal 49 karakter\n";
        cout << "- Tidak boleh mengandung spasi\n";
        return false;
    }
    
    // Cek apakah username sudah terdaftar
    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].getUsername(), username) == 0) {
            cout << "\n[ERROR] Username '" << username << "' sudah terdaftar!\n";
            cout << "Silakan gunakan username lain.\n";
            return false;
        }
    }
    
    cout << "[OK] Username tersedia!\n";
    
    // =================================================================
    // STEP 2: Input Password
    // =================================================================
    cout << "\nPassword (minimal 6 karakter): ";
    cin.getline(password, 50);
    
    if (!isValidPassword(password)) {
        cout << "\n[ERROR] Password terlalu pendek!\n";
        cout << "Password harus minimal 6 karakter.\n";
        return false;
    }
    
    cout << "Konfirmasi Password: ";
    cin.getline(passwordConfirm, 50);
    
    if (strcmp(password, passwordConfirm) != 0) {
        cout << "\n[ERROR] Password tidak cocok!\n";
        cout << "Pastikan password dan konfirmasi password sama.\n";
        return false;
    }
    
    cout << "[OK] Password valid!\n";
    
    // =================================================================
    // STEP 3: Input PIN
    // =================================================================
    cout << "\n=== STEP 2: PIN TRANSAKSI ===\n";
    cout << "\nPIN digunakan untuk keamanan transaksi.\n";
    cout << "PIN (6 digit angka, contoh: 123456): ";
    cin.getline(pin, 7);
    
    if (!isValidPIN(pin)) {
        cout << "\n[ERROR] PIN tidak valid!\n";
        cout << "PIN harus tepat 6 digit angka.\n";
        return false;
    }
    
    cout << "Konfirmasi PIN: ";
    cin.getline(pinConfirm, 7);
    
    if (strcmp(pin, pinConfirm) != 0) {
        cout << "\n[ERROR] PIN tidak cocok!\n";
        cout << "Pastikan PIN dan konfirmasi PIN sama.\n";
        return false;
    }
    
    cout << "[OK] PIN berhasil dibuat!\n";
    
    // =================================================================
    // STEP 4: Input Data KTP
    // =================================================================
    cout << "\n=== STEP 3: DATA KTP ===\n";
    cout << "\nData KTP diperlukan untuk verifikasi identitas.\n";
    
    cout << "\nNIK KTP (16 digit angka): ";
    cin.getline(ktp.nik, 17);
    
    if (!isValidNIK(ktp.nik)) {
        cout << "\n[ERROR] NIK tidak valid!\n";
        cout << "NIK harus tepat 16 digit angka.\n";
        cout << "Contoh: 3201234567890001\n";
        return false;
    }
    
    // Cek apakah NIK sudah terdaftar
    for (int i = 0; i < userCount; i++) {
        KTP existingKTP = users[i].getKTP();
        if (strcmp(existingKTP.nik, ktp.nik) == 0) {
            cout << "\n[ERROR] NIK sudah terdaftar dalam sistem!\n";
            cout << "Satu NIK hanya dapat didaftarkan sekali.\n";
            return false;
        }
    }
    
    cout << "[OK] NIK tersedia!\n";
    
    cout << "\nNama Lengkap (sesuai KTP): ";
    cin.getline(ktp.namaLengkap, 100);
    
    if (strlen(ktp.namaLengkap) < 3) {
        cout << "\n[ERROR] Nama lengkap terlalu pendek!\n";
        return false;
    }
    
    cout << "\nAlamat Lengkap: ";
    cin.getline(ktp.alamat, 200);
    
    if (strlen(ktp.alamat) < 10) {
        cout << "\n[ERROR] Alamat terlalu pendek!\n";
        cout << "Masukkan alamat lengkap Anda.\n";
        return false;
    }
    
    cout << "\nTanggal Lahir (DD/MM/YYYY, contoh: 15/08/1990): ";
    cin.getline(ktp.tanggalLahir, 11);
    
    if (!isValidDate(ktp.tanggalLahir)) {
        cout << "\n[ERROR] Format tanggal lahir tidak valid!\n";
        cout << "Gunakan format DD/MM/YYYY (contoh: 15/08/1990)\n";
        return false;
    }
    
    cout << "[OK] Data KTP lengkap!\n";
    
    // =================================================================
    // STEP 5: Konfirmasi Data
    // =================================================================
    cout << "\n========================================\n";
    cout << "   KONFIRMASI DATA REGISTRASI\n";
    cout << "========================================\n";
    
    cout << "\n=== Data Login ===\n";
    cout << "Username       : " << username << "\n";
    cout << "Password       : " << string(strlen(password), '*') << " (disembunyikan)\n";
    cout << "PIN            : " << string(6, '*') << " (disembunyikan)\n";
    
    cout << "\n=== Data KTP ===\n";
    cout << "NIK            : " << ktp.nik << "\n";
    cout << "Nama Lengkap   : " << ktp.namaLengkap << "\n";
    cout << "Alamat         : " << ktp.alamat << "\n";
    cout << "Tanggal Lahir  : " << ktp.tanggalLahir << "\n";
    
    cout << "\n=== Informasi Akun ===\n";
    cout << "Saldo Awal     : Rp 0.00\n";
    cout << "Status         : Aktif setelah registrasi\n";
    
    cout << "\n========================================\n";
    cout << "Apakah semua data sudah benar? (y/n): ";
    
    char confirm;
    cin >> confirm;
    cin.ignore();
    
    if (confirm != 'y' && confirm != 'Y') {
        cout << "\n[INFO] Registrasi dibatalkan oleh user.\n";
        return false;
    }
    
    // =================================================================
    // STEP 6: Proses Registrasi - Tambah User ke Database
    // =================================================================
    cout << "\n[INFO] Memproses registrasi...\n";
    
    // Buat array baru dengan ukuran +1
    User* newUsers = new User[userCount + 1];
    
    // Copy semua user lama ke array baru
    for (int i = 0; i < userCount; i++) {
        newUsers[i] = users[i];
    }
    
    // Tambah user baru dengan saldo awal 0
    newUsers[userCount] = User(username, password, pin, 0.0);
    newUsers[userCount].setKTP(ktp);
    
    // Hapus array lama dan gunakan array baru
    delete[] users;
    users = newUsers;
    userCount++;
    
    // Simpan ke file users.txt
    fileHandler.saveUsers(users, userCount);
    
    // =================================================================
    // STEP 7: Tampilkan Pesan Sukses
    // =================================================================
    cout << "\n========================================\n";
    cout << "   REGISTRASI BERHASIL!\n";
    cout << "========================================\n";
    
    cout << "\n[SUCCESS] Akun Anda berhasil dibuat!\n";
    cout << "\n=== Detail Akun ===\n";
    cout << "Username       : " << username << "\n";
    cout << "Nama Lengkap   : " << ktp.namaLengkap << "\n";
    cout << "NIK            : " << ktp.nik << "\n";
    cout << "Saldo Awal     : Rp 0.00\n";
    cout << "Status         : Aktif\n";
    
    cout << "\n=== Langkah Selanjutnya ===\n";
    cout << "1. Login menggunakan username dan password Anda\n";
    cout << "2. Lakukan Top Up untuk menambah saldo\n";
    cout << "3. Mulai bertransaksi!\n";
    
    cout << "\n[INFO] Data Anda telah tersimpan dengan aman.\n";
    
    return true;
}

// =================================================================
// FUNGSI USER - AJUKAN REFUND (Push ke Stack)
// =================================================================
void userAjukanRefund(User* user) {
    clearScreen();
    cout << "\n=== AJUKAN REFUND ===\n";
    
    RefundRequest req;
    strcpy(req.username, user->getUsername());
    
    cout << "Masukkan Transaction ID yang ingin di-refund: ";
    cin.getline(req.transactionId, 20);
    
    cout << "Jumlah refund: Rp ";
    if (!(cin >> req.jumlah) || req.jumlah <= 0) {
        cout << "Input tidak valid!\n";
        cin.clear();
        cin.ignore(10000, '\n');
        return;
    }
    cin.ignore();
    
    cout << "Alasan refund: ";
    cin.getline(req.alasan, 200);
    
    generateTimestamp(req.timestamp);
    strcpy(req.status, "Pending");
    
    // Push ke Stack global
    globalRefundStack.push(req);
    
    cout << "\n[SUCCESS] Refund request berhasil diajukan!\n";
    cout << "Request Anda akan diproses oleh Admin.\n";
    cout << "\nKONSEP STACK (LIFO):\n";
    cout << "- Refund request masuk ke Stack\n";
    cout << "- Admin memproses dari atas Stack (LIFO)\n";
    cout << "- Request terakhir yang masuk diproses terlebih dahulu\n";
    cout << "- Refund terbaru biasanya lebih urgent\n";
}

// =================================================================
// FUNGSI USER - KIRIM KELUHAN CS (Enqueue ke Queue)
// =================================================================
void userKirimKeluhan(User* user) {
    clearScreen();
    cout << "\n=== KIRIM KELUHAN CUSTOMER SERVICE ===\n";
    
    CSTicket ticket;
    strcpy(ticket.username, user->getUsername());
    
    cout << "Tulis keluhan/pertanyaan Anda: ";
    cin.getline(ticket.pesan, 500);
    
    generateTimestamp(ticket.timestamp);
    strcpy(ticket.status, "Open");
    
    // Enqueue ke Queue global
    globalCSQueue.enqueue(ticket);
    
    cout << "\n[SUCCESS] Keluhan berhasil dikirim!\n";
    cout << "\nKONSEP QUEUE (FIFO):\n";
    cout << "- Keluhan masuk ke Queue\n";
    cout << "- Admin memproses dari depan Queue (FIFO)\n";
    cout << "- Keluhan yang masuk pertama dilayani terlebih dahulu\n";
    cout << "- Sistem yang adil: First Come First Serve\n";
}

// =================================================================
// FUNGSI USER - TRANSAKSI (Insert ke AVL Tree)
// =================================================================
void userTopUp(User* user, FileHandler& fileHandler) {
    clearScreen();
    cout << "\n=== TOP UP SALDO ===\n";
    
    double jumlah;
    cout << "Masukkan jumlah top up: Rp ";
    if (!(cin >> jumlah) || jumlah <= 0) {
        cout << "Input tidak valid!\n";
        cin.clear();
        cin.ignore(10000, '\n');
        return;
    }
    cin.ignore();
    
    // Verify PIN
    char pin[7];
    cout << "Masukkan PIN (6 digit): ";
    cin.getline(pin, 7);
    
    if (!user->verifyPIN(pin)) {
        cout << "\n[ERROR] PIN salah!\n";
        return;
    }
    
    // Input keterangan
    char keterangan[200];
    cout << "Keterangan (opsional, ENTER untuk skip): ";
    cin.getline(keterangan, 200);
    
    if (strlen(keterangan) == 0) {
        strcpy(keterangan, "Top up via transfer bank");
    }
    
    // Proses top up
    user->addSaldo(jumlah);
    
    // Buat transaksi dan simpan ke AVL Tree & file
    Transaksi t("Top Up", jumlah, keterangan, 
                "System", user->getUsername());
    
    globalAVLTree.insert(t);  // Insert ke AVL Tree global
    fileHandler.appendTransaction(t);
    
    // HUFFMAN CODING - Kompresi Keterangan
    HuffmanCoding huffman;
    huffman.buildTree(t.keterangan);
    
    char compressed[10000];
    huffman.encode(t.keterangan, compressed);
    
    int originalSize = strlen(t.keterangan);
    int compressedSize = (strlen(compressed) + 7) / 8; // Convert bits to bytes
    
    cout << "\n========== STRUK TRANSAKSI ==========\n";
    cout << "ID         : " << t.id << "\n";
    cout << "Waktu      : " << t.timestamp << "\n";
    cout << "Jenis      : " << t.jenis << "\n";
    cout << "Jumlah     : Rp " << t.jumlah << "\n";
    cout << "Keterangan : " << t.keterangan << "\n";
    cout << "Saldo Akhir: Rp " << user->getSaldo() << "\n";
    cout << "=====================================\n";
    
    cout << "\n=== HUFFMAN COMPRESSION ===\n";
    cout << "Original Size  : " << originalSize << " bytes\n";
    cout << "Compressed     : " << compressedSize << " bytes\n";
    cout << "Saving         : " << (originalSize - compressedSize) << " bytes (";
    cout << fixed << setprecision(1) << (100.0 - 100.0 * compressedSize / originalSize) << "% smaller)\n";
    cout << "\n[INFO] Keterangan dikompresi dengan Huffman Coding!\n";
    cout << "[INFO] Transaksi disimpan ke AVL Tree untuk laporan Admin.\n";
}

void userTransfer(User* user, User* users, int count, FileHandler& fileHandler) {
    clearScreen();
    cout << "\n=== TRANSFER SALDO ===\n";
    
    char targetUsername[50];
    cout << "Username tujuan: ";
    cin.getline(targetUsername, 50);
    
    // Cari user tujuan
    User* target = NULL;
    for (int i = 0; i < count; i++) {
        if (strcmp(users[i].getUsername(), targetUsername) == 0) {
            target = &users[i];
            break;
        }
    }
    
    if (target == NULL) {
        cout << "\n[ERROR] Username tujuan tidak ditemukan!\n";
        return;
    }
    
    if (strcmp(target->getUsername(), user->getUsername()) == 0) {
        cout << "\n[ERROR] Tidak bisa transfer ke diri sendiri!\n";
        return;
    }
    
    double jumlah;
    cout << "Jumlah transfer: Rp ";
    if (!(cin >> jumlah) || jumlah <= 0) {
        cout << "Input tidak valid!\n";
        cin.clear();
        cin.ignore(10000, '\n');
        return;
    }
    cin.ignore();
    
    if (user->getSaldo() < jumlah) {
        cout << "\n[ERROR] Saldo tidak mencukupi!\n";
        return;
    }
    
    // Input keterangan
    char keterangan[200];
    cout << "Keterangan (opsional, ENTER untuk skip): ";
    cin.getline(keterangan, 200);
    
    if (strlen(keterangan) == 0) {
        strcpy(keterangan, "Transfer antar pengguna");
    }
    
    // Verify PIN
    char pin[7];
    cout << "Masukkan PIN (6 digit): ";
    cin.getline(pin, 7);
    
    if (!user->verifyPIN(pin)) {
        cout << "\n[ERROR] PIN salah!\n";
        return;
    }
    
    // Proses transfer
    user->deductSaldo(jumlah);
    target->addSaldo(jumlah);
    
    // Buat transaksi dan simpan
    Transaksi t("Transfer", jumlah, keterangan,
                user->getUsername(), target->getUsername());
    
    globalAVLTree.insert(t);
    fileHandler.appendTransaction(t);
    
    // HUFFMAN CODING - Kompresi Keterangan
    HuffmanCoding huffman;
    huffman.buildTree(t.keterangan);
    
    char compressed[10000];
    huffman.encode(t.keterangan, compressed);
    
    int originalSize = strlen(t.keterangan);
    int compressedSize = (strlen(compressed) + 7) / 8;
    
    cout << "\n========== STRUK TRANSAKSI ==========\n";
    cout << "ID         : " << t.id << "\n";
    cout << "Waktu      : " << t.timestamp << "\n";
    cout << "Dari       : " << user->getUsername() << "\n";
    cout << "Ke         : " << target->getUsername() << "\n";
    cout << "Jumlah     : Rp " << t.jumlah << "\n";
    cout << "Keterangan : " << t.keterangan << "\n";
    cout << "Saldo Akhir: Rp " << user->getSaldo() << "\n";
    cout << "=====================================\n";
    
    cout << "\n=== HUFFMAN COMPRESSION ===\n";
    cout << "Original Size  : " << originalSize << " bytes\n";
    cout << "Compressed     : " << compressedSize << " bytes\n";
    cout << "Saving         : " << (originalSize - compressedSize) << " bytes\n";
}

void userPembayaran(User* user, FileHandler& fileHandler) {
    clearScreen();
    cout << "\n=== PEMBAYARAN TAGIHAN ===\n";
    
    const char* layanan[] = {
        "Listrik PLN", "Air PDAM", "Internet Indihome", 
        "Pulsa Telkomsel", "TV Kabel", "Cicilan Motor"
    };
    
    cout << "Pilih jenis pembayaran:\n";
    for (int i = 0; i < 6; i++) {
        cout << (i+1) << ". " << layanan[i] << "\n";
    }
    cout << "Pilih (1-6): ";
    
    int pilihan;
    if (!(cin >> pilihan) || pilihan < 1 || pilihan > 6) {
        cout << "Pilihan tidak valid!\n";
        cin.clear();
        cin.ignore(10000, '\n');
        return;
    }
    cin.ignore();
    
    double jumlah;
    cout << "Jumlah pembayaran: Rp ";
    if (!(cin >> jumlah) || jumlah <= 0) {
        cout << "Input tidak valid!\n";
        cin.clear();
        cin.ignore(10000, '\n');
        return;
    }
    cin.ignore();
    
    if (user->getSaldo() < jumlah) {
        cout << "\n[ERROR] Saldo tidak mencukupi!\n";
        return;
    }
    
    // Verify PIN
    char pin[7];
    cout << "Masukkan PIN (6 digit): ";
    cin.getline(pin, 7);
    
    if (!user->verifyPIN(pin)) {
        cout << "\n[ERROR] PIN salah!\n";
        return;
    }
    
    // Proses pembayaran
    user->deductSaldo(jumlah);
    
    char keterangan[200];
    sprintf(keterangan, "Pembayaran %s", layanan[pilihan-1]);
    
    Transaksi t("Pembayaran", jumlah, keterangan,
                user->getUsername(), "Merchant");
    
    globalAVLTree.insert(t);
    fileHandler.appendTransaction(t);
    
    cout << "\n========== STRUK TRANSAKSI ==========\n";
    cout << "ID         : " << t.id << "\n";
    cout << "Waktu      : " << t.timestamp << "\n";
    cout << "Jenis      : " << layanan[pilihan-1] << "\n";
    cout << "Jumlah     : Rp " << t.jumlah << "\n";
    cout << "Saldo Akhir: Rp " << user->getSaldo() << "\n";
    cout << "=====================================\n";
}

// =================================================================
// DEMO HUFFMAN CODING - DENGAN PRAKTIK LANGSUNG
// =================================================================
void demoHuffman() {
    clearScreen();
    cout << "\n========================================\n";
    cout << "  HUFFMAN CODING - KOMPRESI TRANSAKSI\n";
    cout << "========================================\n";
    
    cout << "\nPilih mode demo:\n";
    cout << "1. Demo dengan contoh keterangan\n";
    cout << "2. Input keterangan sendiri\n";
    cout << "3. Kompresi semua transaksi di database\n";
    cout << "\nPilih (1-3): ";
    
    int choice;
    if (!(cin >> choice)) {
        cin.clear();
        cin.ignore(10000, '\n');
        return;
    }
    cin.ignore();
    
    if (choice == 1) {
        // DEMO DENGAN CONTOH
        const char* keterangan = "Pembayaran Listrik PLN Bulan Januari 2025";
        cout << "\n=== DEMO KOMPRESI ===\n";
        cout << "String Original: \"" << keterangan << "\"\n";
        cout << "Panjang String : " << strlen(keterangan) << " karakter\n";
        
        HuffmanCoding huffman;
        huffman.buildTree(keterangan);
        
        char encoded[10000];
        huffman.encode(keterangan, encoded);
        
        cout << "\n=== HASIL KOMPRESI ===\n";
        cout << "Kode Binary:\n" << encoded << "\n";
        cout << "\nPanjang: " << strlen(encoded) << " bits\n";
        
        cout << "\n=== TABEL KODE HUFFMAN ===\n";
        huffman.displayKode();
        
        cout << "\n=== ANALISIS EFISIENSI ===\n";
        huffman.calculateEfficiency(keterangan);
        
    } else if (choice == 2) {
        // INPUT SENDIRI
        char keterangan[500];
        cout << "\n=== INPUT KETERANGAN TRANSAKSI ===\n";
        cout << "Masukkan keterangan (max 499 karakter):\n";
        cin.getline(keterangan, 500);
        
        if (strlen(keterangan) < 5) {
            cout << "\n[ERROR] Keterangan terlalu pendek!\n";
            return;
        }
        
        cout << "\n=== PROSES KOMPRESI ===\n";
        cout << "String Original: \"" << keterangan << "\"\n";
        cout << "Panjang: " << strlen(keterangan) << " karakter\n";
        
        HuffmanCoding huffman;
        huffman.buildTree(keterangan);
        
        char encoded[10000];
        huffman.encode(keterangan, encoded);
        
        cout << "\n=== HASIL KOMPRESI ===\n";
        cout << "Kode Binary:\n" << encoded << "\n";
        cout << "\nPanjang: " << strlen(encoded) << " bits\n";
        
        // Hitung ukuran file
        int originalBytes = strlen(keterangan);
        int compressedBits = strlen(encoded);
        int compressedBytes = (compressedBits + 7) / 8; // Round up
        
        cout << "\n=== PERBANDINGAN UKURAN ===\n";
        cout << "Ukuran Original    : " << originalBytes << " bytes\n";
        cout << "Ukuran Compressed  : " << compressedBytes << " bytes\n";
        cout << "Penghematan        : " << (originalBytes - compressedBytes) << " bytes\n";
        cout << "Rasio Kompresi     : " << fixed << setprecision(2) 
             << (100.0 * compressedBytes / originalBytes) << "%\n";
        
        cout << "\n=== TABEL KODE HUFFMAN ===\n";
        huffman.displayKode();
        
    } else if (choice == 3) {
        // KOMPRESI DATABASE
        cout << "\n=== KOMPRESI DATABASE TRANSAKSI ===\n";
        cout << "Menganalisis semua transaksi di AVL Tree...\n";
        
        if (globalAVLTree.isEmpty()) {
            cout << "\n[INFO] Belum ada transaksi di database.\n";
            return;
        }
        
        cout << "\n[INFO] Fitur ini akan mengkompresi semua keterangan transaksi.\n";
        cout << "Dalam implementasi real:\n";
        cout << "- Setiap keterangan transaksi di-encode saat disimpan\n";
        cout << "- Ukuran database berkurang signifikan\n";
        cout << "- Transmisi data lebih cepat\n";
        cout << "- Cost storage server lebih murah\n";
        
        // Simulasi kompresi
        const char* samples[] = {
            "Transfer ke user lain",
            "Pembayaran Listrik PLN",
            "Top up via Bank BCA",
            "Pembayaran PDAM Jakarta",
            "Transfer dana darurat"
        };
        
        int totalOriginal = 0;
        int totalCompressed = 0;
        
        cout << "\n=== SIMULASI KOMPRESI 5 TRANSAKSI ===\n";
        for (int i = 0; i < 5; i++) {
            HuffmanCoding huffman;
            huffman.buildTree(samples[i]);
            
            char encoded[10000];
            huffman.encode(samples[i], encoded);
            
            int origSize = strlen(samples[i]);
            int compSize = (strlen(encoded) + 7) / 8;
            
            totalOriginal += origSize;
            totalCompressed += compSize;
            
            cout << "\n[" << (i+1) << "] \"" << samples[i] << "\"\n";
            cout << "    Original: " << origSize << " bytes ? Compressed: " 
                 << compSize << " bytes (";
            cout << fixed << setprecision(1) << (100.0 * compSize / origSize) << "%)\n";
        }
        
        cout << "\n=== TOTAL PENGHEMATAN ===\n";
        cout << "Total Original     : " << totalOriginal << " bytes\n";
        cout << "Total Compressed   : " << totalCompressed << " bytes\n";
        cout << "Total Penghematan  : " << (totalOriginal - totalCompressed) << " bytes\n";
        cout << "Efisiensi          : " << fixed << setprecision(2) 
             << (100.0 - 100.0 * totalCompressed / totalOriginal) << "% lebih kecil\n";
        
        cout << "\n=== PROYEKSI DATABASE ===\n";
        cout << "Jika database memiliki 1 juta transaksi:\n";
        cout << "- Penghematan: ~" << ((totalOriginal - totalCompressed) * 1000000 / 5 / 1024 / 1024) 
             << " MB\n";
        cout << "- Backup lebih cepat\n";
        cout << "- Biaya cloud storage lebih murah\n";
    }
    
    cout << "\n=== CARA KERJA HUFFMAN CODING ===\n";
    cout << "1. Hitung frekuensi setiap karakter\n";
    cout << "2. Bangun Min Heap berdasarkan frekuensi\n";
    cout << "3. Ekstrak 2 node terkecil, gabungkan\n";
    cout << "4. Ulangi sampai tersisa 1 node (root)\n";
    cout << "5. Generate kode: kiri=0, kanan=1\n";
    cout << "6. Karakter sering muncul = kode pendek\n";
    
    cout << "\n=== KEUNTUNGAN DALAM E-WALLET ===\n";
    cout << "+ Hemat storage database (hingga 40-60%)\n";
    cout << "+ Transmisi data lebih cepat\n";
    cout << "+ Backup/restore lebih efisien\n";
    cout << "+ Biaya server lebih murah\n";
    cout << "+ Tetap lossless (data tidak hilang)\n";
}

// =================================================================
// MENU USER
// =================================================================
void userMenu(User* user, User* users, int count, FileHandler& fileHandler) {
    while (true) {
        clearScreen();
        cout << "\n========================================\n";
        cout << "   MENU USER - E-WALLET\n";
        cout << "========================================\n";
        cout << "Username: " << user->getUsername() << "\n";
        cout << "Saldo   : Rp " << fixed << setprecision(2) << user->getSaldo() << "\n";
        
        cout << "\n=== MENU UTAMA ===\n";
        cout << "1. Top Up Saldo\n";
        cout << "2. Transfer ke Pengguna Lain\n";
        cout << "3. Pembayaran Tagihan\n";
        cout << "4. Ajukan Refund (Stack - LIFO)\n";
        cout << "5. Kirim Keluhan Customer Service (Queue - FIFO)\n";
        cout << "6. Lihat Info Lengkap (termasuk KTP)\n";
        cout << "7. Demo Huffman Coding\n";
        cout << "8. Logout\n";
        cout << "\nPilih menu (1-8): ";
        
        int choice;
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }
        cin.ignore();
        
        switch (choice) {
            case 1:
                userTopUp(user, fileHandler);
                break;
            case 2:
                userTransfer(user, users, count, fileHandler);
                break;
            case 3:
                userPembayaran(user, fileHandler);
                break;
            case 4:
                userAjukanRefund(user);
                break;
            case 5:
                userKirimKeluhan(user);
                break;
            case 6:
                user->displayInfo();
                break;
            case 7:
                demoHuffman();
                break;
            case 8:
                fileHandler.saveUsers(users, count);
                cout << "\nData tersimpan. Logout berhasil.\n";
                return;
            default:
                cout << "\nPilihan tidak valid!\n";
        }
        
        cout << "\nTekan ENTER untuk kembali ke menu...";
        cin.get();
    }
}

// =================================================================
// FUNGSI LOGIN (User atau Admin)
// =================================================================
int loginSystem(User* users, int count, User** loggedUser, FileHandler& fileHandler, char* adminID) {
    char username[50], password[50];
    
    cout << "Username: ";
    cin.getline(username, 50);
    cout << "Password: ";
    cin.getline(password, 50);
    
    // Cek apakah login sebagai admin
    if (fileHandler.loadAdmin(username, password, adminID)) {
        cout << "\n[SUCCESS] Login berhasil sebagai Admin!\n";
        return 2; // Return 2 untuk admin
    }
    
    // Cek apakah login sebagai user biasa
    for (int i = 0; i < count; i++) {
        if (users[i].authenticate(username, password)) {
            *loggedUser = &users[i];
            cout << "\n[SUCCESS] Login berhasil sebagai User!\n";
            return 1; // Return 1 untuk user
        }
    }
    
    return 0; // Return 0 untuk login gagal
}

// =================================================================
// MAIN FUNCTION
// =================================================================
int main() {
    clearScreen();
    
    cout << "\n";
    cout << "========================================\n";
    cout << "   SISTEM E-WALLET\n";
    cout << "   SIMULASI DOMPET DIGITAL\n";
    cout << "========================================\n";
    cout << "\n";
    
    // Inisialisasi File Handler
    FileHandler fileHandler;
    fileHandler.initializeFiles();
    
    // Load data
    cout << "Memuat data dari database...\n";
    User* users = NULL;
    int userCount = fileHandler.loadUsers(users);
    fileHandler.loadTransactions(globalAVLTree);
    
    cout << "\nData berhasil dimuat!\n";
    cout << "Total User: " << userCount << "\n";
    cout << "Transaksi dimuat ke AVL Tree.\n";
    
    pause();
    
    // Main Loop
    while (true) {
        clearScreen();
        cout << "\n========================================\n";
        cout << "   E-WALLET SYSTEM\n";
        cout << "   MENU UTAMA\n";
        cout << "========================================\n";
        cout << "\n1. Login (User / Admin)\n";
        cout << "2. Registrasi User Baru\n";
        cout << "3. Demo Huffman Coding\n";
        cout << "4. Keluar\n";
        cout << "\nPilih (1-4): ";
        
        int choice;
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }
        cin.ignore();
        
        if (choice == 1) {
            // LOGIN
            clearScreen();
            cout << "\n========================================\n";
            cout << "   LOGIN SISTEM\n";
            cout << "========================================\n";
            cout << "\nMasukkan kredensial Anda\n";
            cout << "(Username 'admin' untuk akses administrator)\n\n";
            
            User* loggedUser = NULL;
            char adminID[20];
            
            int loginResult = loginSystem(users, userCount, &loggedUser, fileHandler, adminID);
            
            if (loginResult == 1) {
                // Login sebagai User
                pause();
                userMenu(loggedUser, users, userCount, fileHandler);
                
            } else if (loginResult == 2) {
                // Login sebagai Admin
                Admin admin("admin", "admin123", adminID);
                pause();
                adminMenu(&admin, globalRefundStack, globalCSQueue, 
                         globalAVLTree, users, userCount);
                fileHandler.saveUsers(users, userCount);
                
            } else {
                cout << "\n[ERROR] Username atau password salah!\n";
                pause();
            }
            
        } else if (choice == 2) {
            // REGISTRASI USER BARU
            if (registerUser(users, userCount, fileHandler)) {
                pause();
            } else {
                cout << "\n[INFO] Registrasi dibatalkan atau gagal.\n";
                pause();
            }
            
        } else if (choice == 3) {
            // DEMO HUFFMAN CODING
            demoHuffman();
            pause();
            
        } else if (choice == 4) {
            // KELUAR
            cout << "\n========================================\n";
            cout << "Menyimpan data ke database...\n";
            fileHandler.saveUsers(users, userCount);
            cout << "Data berhasil disimpan!\n";
            cout << "\nTerima kasih telah menggunakan E-Wallet System.\n";
            cout << "========================================\n\n";
            break;
            
        } else {
            cout << "\n[ERROR] Pilihan tidak valid!\n";
            pause();
        }
    }
    
    // Cleanup
    delete[] users;
    
    return 0;
}
