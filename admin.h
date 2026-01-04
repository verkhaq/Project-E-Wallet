#ifndef ADMIN_H
#define ADMIN_H

#include <iostream>
#include <iomanip>
#include <cstring>
#include "user.h"
#include "data_structures.h"

using namespace std;

/*
 * =====================================================================
 * CLASS ADMIN - Pengelola Sistem E-Wallet
 * =====================================================================
 * 
 * INHERITANCE:
 * Admin adalah turunan dari User, namun TIDAK memiliki:
 * - Saldo (Admin tidak bisa transaksi)
 * - PIN (tidak perlu untuk admin)
 * - KTP (hanya untuk user biasa)
 * 
 * PERAN ADMIN:
 * - Melihat laporan transaksi global
 * - Memproses refund request dari Stack (LIFO)
 * - Memproses keluhan customer dari Queue (FIFO)
 * - Monitoring sistem
 * 
 * CARA KERJA AKSES STRUKTUR DATA GLOBAL:
 * 
 * 1. REFUND STACK (LIFO - Last In First Out):
 *    - User ajukan refund -> push() ke globalRefundStack (di main.cpp)
 *    - Admin proses refund -> pop() dari globalRefundStack
 *    - Refund TERAKHIR yang masuk diproses TERLEBIH DAHULU
 *    - Logika: Refund terbaru biasanya lebih urgent
 * 
 * 2. CS QUEUE (FIFO - First In First Out):
 *    - User kirim keluhan -> enqueue() ke globalCSQueue (di main.cpp)
 *    - Admin proses keluhan -> dequeue() dari globalCSQueue
 *    - Keluhan yang masuk PERTAMA diproses TERLEBIH DAHULU
 *    - Logika: Fair system - siapa cepat dia dilayani dulu
 * 
 * 3. AVL TREE (Self-Balancing BST):
 *    - Semua transaksi sistem disimpan di globalAVLTree (di main.cpp)
 *    - Admin dapat query transaksi berdasarkan rentang jumlah
 *    - AVL Tree menjaga balance otomatis -> pencarian O(log n)
 * 
 * SINKRONISASI:
 * - Struktur data dibuat sebagai variabel GLOBAL di main.cpp
 * - Admin menerima REFERENCE (pointer) ke struktur tersebut
 * - Perubahan yang dilakukan Admin langsung terlihat oleh User
 * - Tidak ada duplikasi data - single source of truth
 * 
 * =====================================================================
 */

// =================================================================
// CLASS ADMIN (Turunan dari User - Inheritance)
// =================================================================
class Admin {
private:
    char username[50];
    char password[50];
    char adminID[20];
    
public:
    Admin() {
        strcpy(username, "");
        strcpy(password, "");
        strcpy(adminID, "");
    }
    
    Admin(const char* u, const char* p, const char* id) {
        strcpy(username, u);
        strcpy(password, p);
        strcpy(adminID, id);
    }
    
    void displayInfo() const {
        cout << "\n      === Informasi Admin ===\n";
        cout << "      Username  : " << username << "\n";
        cout << "      Admin ID  : " << adminID << "\n";
        cout << "      Role      : Administrator\n";
        cout << "      Akses     : Full System Management\n";
        cout << "\n      CATATAN:\n";
        cout << "      - Admin TIDAK memiliki saldo (tidak bisa transaksi)\n";
        cout << "      - Admin hanya mengelola sistem dan user\n";
    }
    
    const char* getUsername() const {
        return username;
    }
    
    const char* getAdminID() const {
        return adminID;
    }
    
    // =================================================================
    // FITUR 1: PROSES REFUND REQUEST (Stack LIFO)
    // =================================================================
    /*
     * PROSES REFUND DARI STACK
     * 
     * PARAMETER:
     * - refundStack: Reference ke Stack global (dari main.cpp)
     * - users: Array user untuk update saldo jika approved
     * - userCount: Jumlah user
     * 
     * ALUR KERJA:
     * 1. User mengalami transaksi gagal/error
     * 2. User ajukan refund -> push() ke Stack global
     * 3. Refund masuk ke Stack dengan konsep LIFO
     * 4. Admin panggil fungsi ini -> pop() dari Stack
     * 5. Refund TERAKHIR yang masuk (Top) diproses dulu
     * 6. Admin approve/reject -> update saldo user jika approved
     * 
     * KENAPA LIFO?
     * - Refund terbaru biasanya lebih urgent
     * - User baru saja mengalami masalah -> butuh penanganan cepat
     * - Stack cocok untuk operasi "undo" atau "cancel terbaru"
     */
    void processRefund(RefundStack& refundStack, User* users, int userCount) {
        cout << "\n      ========================================\n";
        cout << "      PROSES REFUND REQUEST (STACK - LIFO)\n";
        cout << "      ========================================\n";
        
        refundStack.displayAll();
        
        if (refundStack.isEmpty()) {
            return;
        }
        
        cout << "\n      Proses refund teratas (yang terakhir masuk)? (y/n): ";
        char choice;
        cin >> choice;
        cin.ignore();
        
        if (choice == 'y' || choice == 'Y') {
            // Pop dari Stack (LIFO)
            RefundRequest req = refundStack.pop();
            
            cout << "\n      === Memproses Refund ===\n";
            cout << "      Username      : " << req.username << "\n";
            cout << "      Transaction   : " << req.transactionId << "\n";
            cout << "      Jumlah        : Rp " << req.jumlah << "\n";
            cout << "      Alasan        : " << req.alasan << "\n";
            
            cout << "\n      Approve refund? (y=Approve, n=Reject): ";
            char approve;
            cin >> approve;
            cin.ignore();
            
            bool found = false;
            for (int i = 0; i < userCount; i++) {
                if (strcmp(users[i].getUsername(), req.username) == 0) {
                    if (approve == 'y' || approve == 'Y') {
                        users[i].addSaldo(req.jumlah);
                        cout << "\n      [APPROVED] Refund Rp " << req.jumlah 
                             << " berhasil dikembalikan ke " << req.username << "\n";
                        cout << "      Saldo baru: Rp " << users[i].getSaldo() << "\n";
                    } else {
                        cout << "\n      [REJECTED] Refund request ditolak.\n";
                    }
                    found = true;
                    break;
                }
            }
            
            if (!found) {
                cout << "\n      Error: Username tidak ditemukan!\n";
            }
            
            cout << "\n      === PENJELASAN STACK (LIFO) ===\n";
            cout << "      - Last In First Out: Yang terakhir masuk diproses dulu\n";
            cout << "      - Push: User ajukan refund (masuk ke Stack)\n";
            cout << "      - Pop: Admin proses refund teratas (keluar dari Stack)\n";
            cout << "      - Peek: Lihat refund teratas tanpa menghapus\n";
            cout << "      - Refund terbaru biasanya lebih urgent\n";
        }
    }
    
    // =================================================================
    // FITUR 2: PROSES CUSTOMER SERVICE (Queue FIFO)
    // =================================================================
    /*
     * PROSES KELUHAN DARI QUEUE
     * 
     * PARAMETER:
     * - csQueue: Reference ke Queue global (dari main.cpp)
     * 
     * ALUR KERJA:
     * 1. User kirim keluhan/pertanyaan
     * 2. Keluhan masuk -> enqueue() ke Queue global
     * 3. Queue menyimpan keluhan dengan konsep FIFO
     * 4. Admin panggil fungsi ini -> dequeue() dari Queue
     * 5. Keluhan yang masuk PERTAMA diproses TERLEBIH DAHULU
     * 6. Admin baca & jawab keluhan
     * 
     * KENAPA FIFO?
     * - Fair system: First Come First Serve
     * - User yang menunggu lebih lama dilayani dulu
     * - Mencegah "queue jumping" atau ketidakadilan
     * - Cocok untuk sistem customer service yang adil
     * 
     * PERBEDAAN DENGAN PRIORITY QUEUE:
     * - Queue biasa: Berdasarkan URUTAN MASUK (waktu)
     * - Priority Queue: Berdasarkan PRIORITAS (urgent/tidak)
     * - Di sini kita pakai Queue biasa (FIFO)
     */
    void processCSTicket(CSQueue& csQueue) {
        cout << "\n      ========================================\n";
        cout << "      PROSES CUSTOMER SERVICE\n";
        cout << "      (QUEUE - FIFO / First Come First Serve)\n";
        cout << "      ========================================\n";
        
        csQueue.displayAll();
        
        if (csQueue.isEmpty()) {
            return;
        }
        
        cout << "\n      Proses keluhan yang paling depan (masuk pertama)? (y/n): ";
        char choice;
        cin >> choice;
        cin.ignore();
        
        if (choice == 'y' || choice == 'Y') {
            // Dequeue dari Queue (FIFO)
            CSTicket ticket = csQueue.dequeue();
            
            cout << "\n      === Memproses Tiket CS ===\n";
            cout << "      Username      : " << ticket.username << "\n";
            cout << "      Pesan         : " << ticket.pesan << "\n";
            cout << "      Timestamp     : " << ticket.timestamp << "\n";
            cout << "      Status        : " << ticket.status << "\n";
            
            cout << "\n      Masukkan balasan Admin: ";
            char balasan[500];
            cin.getline(balasan, 500);
            
            cout << "\n      [RESOLVED] Tiket berhasil diproses dan dijawab.\n";
            cout << "      Balasan dikirim ke: " << ticket.username << "\n";
            
            cout << "\n      === PENJELASAN QUEUE (FIFO) ===\n";
            cout << "      - First In First Out: Yang masuk pertama dilayani dulu\n";
            cout << "      - Enqueue: User kirim keluhan (masuk ke belakang Queue)\n";
            cout << "      - Dequeue: Admin proses keluhan depan (keluar dari Queue)\n";
            cout << "      - Front: Keluhan yang paling depan (masuk pertama)\n";
            cout << "      - Rear: Keluhan yang paling belakang (masuk terakhir)\n";
            cout << "      - Sistem yang adil: siapa cepat dia dapat\n";
        }
    }
    
    // =================================================================
    // FITUR 3: LAPORAN TRANSAKSI (AVL Tree)
    // =================================================================
    /*
     * LAPORAN TRANSAKSI DARI AVL TREE
     * 
     * PARAMETER:
     * - avlTree: Reference ke AVL Tree global (dari main.cpp)
     * 
     * ALUR KERJA:
     * 1. Setiap transaksi user disimpan ke AVL Tree
     * 2. AVL Tree menjaga keseimbangan otomatis (Balance Factor <= 1)
     * 3. Admin dapat query transaksi berdasarkan rentang jumlah
     * 4. Pencarian O(log n) - sangat cepat!
     * 
     * KEUNTUNGAN AVL TREE:
     * - Selalu seimbang (tidak akan jadi skewed)
     * - Pencarian, insert, delete: O(log n) - DIJAMIN
     * - Cocok untuk database dengan banyak query
     * - 4 jenis rotasi menjaga keseimbangan: LL, RR, LR, RL
     * 
     * FITUR LAPORAN:
     * 1. Lihat semua transaksi (inorder - terurut)
     * 2. Filter berdasarkan rentang jumlah (min-max)
     * 3. Visualisasi struktur tree
     * 4. Info statistik (tinggi, rotasi, balance factor)
     */
    void viewTransactionReport(AVLTree& avlTree) {
        cout << "\n      ========================================\n";
        cout << "      LAPORAN TRANSAKSI SISTEM\n";
        cout << "      (AVL TREE - SELF-BALANCING BST)\n";
        cout << "      ========================================\n";
        
        cout << "\n      Pilih jenis laporan:\n";
        cout << "      1. Lihat Semua Transaksi (Inorder)\n";
        cout << "      2. Filter Berdasarkan Rentang Jumlah\n";
        cout << "      3. Visualisasi AVL Tree\n";
        cout << "      4. Informasi AVL Tree (Statistik)\n";
        cout << "      Pilih (1-4): ";
        
        int choice;
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(10000, '\n');
            return;
        }
        cin.ignore();
        
        if (choice == 1) {
            cout << "\n      === Semua Transaksi (Urut Berdasarkan Jumlah) ===\n";
            avlTree.displayInorder();
            
        } else if (choice == 2) {
            double minVal, maxVal;
            cout << "\n      Masukkan jumlah minimum: Rp ";
            if (!(cin >> minVal)) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "      Input tidak valid!\n";
                return;
            }
            cout << "      Masukkan jumlah maksimum: Rp ";
            if (!(cin >> maxVal)) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "      Input tidak valid!\n";
                return;
            }
            cin.ignore();
            
            cout << "\n      === Transaksi Rp" << minVal << " - Rp" << maxVal << " ===\n";
            avlTree.searchByRange(minVal, maxVal);
            
            cout << "\n      EFISIENSI PENCARIAN AVL TREE:\n";
            cout << "      - Kompleksitas: O(log n) - sangat cepat!\n";
            cout << "      - Tree selalu seimbang (|Balance Factor| <= 1)\n";
            cout << "      - Cocok untuk database dengan banyak query\n";
            
        } else if (choice == 3) {
            avlTree.visualize();
            cout << "\n      Keterangan:\n";
            cout << "      - Angka = Jumlah transaksi\n";
            cout << "      - BF = Balance Factor (Height Left - Height Right)\n";
            cout << "      - AVL Tree SELALU menjaga |BF| <= 1\n";
            
        } else if (choice == 4) {
            avlTree.displayInfo();
        }
    }
    
    // =================================================================
    // FITUR 4: LIHAT SEMUA USER
    // =================================================================
    void viewAllUsers(User* users, int count) {
        cout << "\n      ========================================\n";
        cout << "      DAFTAR SEMUA USER\n";
        cout << "      ========================================\n";
        
        cout << "\n      Total User Terdaftar: " << count << "\n\n";
        
        for (int i = 0; i < count; i++) {
            cout << "      [" << (i+1) << "] " << users[i].getUsername() << "\n";
            cout << "          Saldo: Rp " << users[i].getSaldo() << "\n";
            KTP ktp = users[i].getKTP();
            cout << "          NIK  : " << ktp.nik << "\n";
            cout << "          Nama : " << ktp.namaLengkap << "\n";
            cout << "\n";
        }
    }
    
    // =================================================================
    // FITUR 5: STATISTIK SISTEM
    // =================================================================
    void viewSystemStatistics(RefundStack& refundStack, CSQueue& csQueue, 
                              AVLTree& avlTree, int userCount) {
        cout << "\n      ========================================\n";
        cout << "      STATISTIK SISTEM E-WALLET\n";
        cout << "      ========================================\n";
        
        cout << "\n      === Pengguna ===\n";
        cout << "      Total User          : " << userCount << " user\n";
        
        cout << "\n      === Transaksi (AVL Tree) ===\n";
        if (!avlTree.isEmpty()) {
            avlTree.displayInfo();
        } else {
            cout << "      Belum ada transaksi.\n";
        }
        
        cout << "\n      === Refund Request (Stack - LIFO) ===\n";
        cout << "      Total Request       : " << refundStack.getSize() << " request\n";
        cout << "      Status              : " << (refundStack.isEmpty() ? "Kosong" : "Ada yang pending") << "\n";
        cout << "      Konsep              : Last In First Out (LIFO)\n";
        
        cout << "\n      === Customer Service (Queue - FIFO) ===\n";
        cout << "      Total Tiket         : " << csQueue.getSize() << " tiket\n";
        cout << "      Status              : " << (csQueue.isEmpty() ? "Tidak ada tiket" : "Ada tiket pending") << "\n";
        cout << "      Konsep              : First In First Out (FIFO)\n";
        
        cout << "\n      STRUKTUR DATA YANG DIGUNAKAN:\n";
        cout << "      1. Stack (LIFO)    : Refund Request (terakhir masuk diproses dulu)\n";
        cout << "      2. Queue (FIFO)    : Customer Service (pertama masuk dilayani dulu)\n";
        cout << "      3. AVL Tree (BST)  : Transaksi Global (selalu seimbang)\n";
        cout << "      4. Huffman Coding  : Kompresi keterangan transaksi\n";
    }
};

// =================================================================
// FUNGSI MENU ADMIN (Dipanggil dari main.cpp)
// =================================================================
/*
 * MENU ADMIN - Interface untuk Administrator
 * 
 * Fungsi ini menerima REFERENCE ke semua struktur data global:
 * - refundStack: Stack untuk refund (LIFO)
 * - csQueue: Queue untuk customer service (FIFO)
 * - avlTree: AVL Tree untuk transaksi
 * - users: Array user untuk update saldo
 * 
 * Admin TIDAK membuat struktur data baru, hanya mengakses yang sudah ada.
 * Perubahan yang dilakukan Admin langsung tersinkronisasi ke User.
 */
void adminMenu(Admin* admin, RefundStack& refundStack, CSQueue& csQueue,
               AVLTree& avlTree, User* users, int userCount) {
    
    while (true) {
        // Clear screen
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
        
        cout << "\n";
        cout << "========================================\n";
        cout << "   MENU ADMIN - E-WALLET SYSTEM\n";
        cout << "========================================\n";
        admin->displayInfo();
        
        cout << "\n=== MENU UTAMA ===\n";
        cout << "1. Proses Refund Request (Stack - LIFO)\n";
        cout << "2. Proses Customer Service (Queue - FIFO)\n";
        cout << "3. Lihat Laporan Transaksi (AVL Tree)\n";
        cout << "4. Lihat Semua User\n";
        cout << "5. Statistik Sistem\n";
        cout << "6. Logout\n";
        cout << "\nPilih menu (1-6): ";
        
        int choice;
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }
        cin.ignore();
        
        switch (choice) {
            case 1:
                admin->processRefund(refundStack, users, userCount);
                break;
            case 2:
                admin->processCSTicket(csQueue);
                break;
            case 3:
                admin->viewTransactionReport(avlTree);
                break;
            case 4:
                admin->viewAllUsers(users, userCount);
                break;
            case 5:
                admin->viewSystemStatistics(refundStack, csQueue, avlTree, userCount);
                break;
            case 6:
                cout << "\nLogout berhasil. Kembali ke menu login...\n";
                return;
            default:
                cout << "\nPilihan tidak valid!\n";
        }
        
        cout << "\nTekan ENTER untuk kembali ke menu admin...";
        cin.get();
    }
}

#endif
