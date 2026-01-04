#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include <fstream>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include "user.h"
#include "data_structures.h"

using namespace std;

// =================================================================
// CLASS FILE HANDLER - Manajemen File .txt Database
// =================================================================
class FileHandler {
private:
    const char* userFile;
    const char* adminFile;
    const char* transactionFile;
    
    void generateNIK(char* nik) {
        for (int i = 0; i < 16; i++) {
            nik[i] = '0' + (rand() % 10);
        }
        nik[16] = '\0';
    }
    
    void generateDate(char* date) {
        int day = 1 + (rand() % 28);
        int month = 1 + (rand() % 12);
        int year = 1970 + (rand() % 35);
        sprintf(date, "%02d/%02d/%04d", day, month, year);
    }
    
public:
    FileHandler() {
        userFile = "users.txt";
        adminFile = "admins.txt";
        transactionFile = "transactions.txt";
        srand(time(0));
    }
    
    void initializeFiles() {
        ifstream checkUser(userFile);
        bool userExists = false;
        
        if (checkUser.is_open()) {
            char line[500];
            if (checkUser.getline(line, 500)) {
                userExists = true;
            }
            checkUser.close();
        }
        
        if (!userExists) {
            cout << "\n=== Inisialisasi Database ===\n";
            cout << "Membuat dummy data...\n";
            createDummyUsers();
            createDummyAdmins();
            createDummyTransactions();
            cout << "Database berhasil dibuat!\n\n";
        }
    }
    
    void createDummyAdmins() {
        ofstream file(adminFile);
        if (!file.is_open()) {
            cout << "Error: Tidak bisa membuat file admins.txt\n";
            return;
        }
        
        file << "admin|admin123|ADM001\n";
        file << "superadmin|super123|ADM002\n";
        
        file.close();
    }
    
    void createDummyUsers() {
        ofstream file(userFile);
        if (!file.is_open()) {
            cout << "Error: Tidak bisa membuat file users.txt\n";
            return;
        }
        
        const char* names[] = {
            "Budi Santoso", "Siti Nurhaliza", "Ahmad Fauzi", "Dewi Lestari",
            "Eko Prasetyo", "Fitri Handayani", "Gani Permana", "Hana Pertiwi",
            "Irfan Hakim", "Julia Perez", "Kurnia Sari", "Lina Marlina",
            "Made Wirawan", "Nina Zatulini", "Oki Setiana", "Putri Ayu",
            "Qori Sandioriva", "Rina Nose", "Surya Saputra", "Tia Ivanka"
        };
        
        const char* cities[] = {
            "Jakarta", "Bandung", "Surabaya", "Yogyakarta", "Semarang",
            "Medan", "Makassar", "Depok", "Tangerang", "Palembang"
        };
        
        for (int i = 0; i < 20; i++) {
            char nik[17];
            char dob[11];
            generateNIK(nik);
            generateDate(dob);
            
            double saldo = 100000 + (rand() % 10000000);
            
            file << "user" << (i+1) << "|pass" << (i+1) << "|123456|"
                 << saldo << "|"
                 << nik << "|" << names[i] << "|"
                 << "Jl. Contoh No." << (i+1) << ", " << cities[i % 10] << "|"
                 << dob << "\n";
        }
        
        file.close();
    }
    
    void createDummyTransactions() {
        ofstream file(transactionFile);
        if (!file.is_open()) {
            cout << "Error: Tidak bisa membuat file transactions.txt\n";
            return;
        }
        
        const char* jenis[] = {"Top Up", "Transfer", "Pembayaran"};
        const char* keterangan[] = {
            "Top up via Bank BCA",
            "Transfer ke user lain",
            "Pembayaran Listrik PLN",
            "Pembayaran PDAM",
            "Top up via Indomaret",
            "Pembayaran Pulsa Telkomsel",
            "Transfer antar user",
            "Pembayaran Internet"
        };
        
        for (int i = 0; i < 30; i++) {
            time_t now = time(0);
            struct tm* t = localtime(&now);
            t->tm_mday -= (rand() % 30);
            mktime(t);
            
            char timestamp[20];
            sprintf(timestamp, "%02d/%02d/%04d %02d:%02d:%02d",
                    t->tm_mday, t->tm_mon + 1, t->tm_year + 1900,
                    rand() % 24, rand() % 60, rand() % 60);
            
            int typeIdx = rand() % 3;
            const char* type = jenis[typeIdx];
            
            char from[50], to[50];
            double amount = 10000 + (rand() % 1000000);
            
            if (strcmp(type, "Top Up") == 0) {
                strcpy(from, "System");
                sprintf(to, "user%d", (rand() % 20) + 1);
            } else if (strcmp(type, "Transfer") == 0) {
                sprintf(from, "user%d", (rand() % 20) + 1);
                sprintf(to, "user%d", (rand() % 20) + 1);
            } else {
                sprintf(from, "user%d", (rand() % 20) + 1);
                strcpy(to, "Merchant");
            }
            
            file << "TRX" << (1000 + i) << "|" << timestamp << "|" << type << "|"
                 << from << "|" << to << "|" << amount << "|"
                 << keterangan[rand() % 8] << "\n";
        }
        
        file.close();
    }
    
    int loadUsers(User*& users) {
        ifstream file(userFile);
        if (!file.is_open()) {
            return 0;
        }
        
        int count = 0;
        char line[500];
        while (file.getline(line, 500)) {
            count++;
        }
        
        file.clear();
        file.seekg(0);
        
        users = new User[count];
        
        int idx = 0;
        while (file.getline(line, 500)) {
            char username[50], password[50], pin[7];
            double saldo;
            KTP ktp;
            
            char* token = strtok(line, "|");
            strcpy(username, token);
            
            token = strtok(NULL, "|");
            strcpy(password, token);
            
            token = strtok(NULL, "|");
            strcpy(pin, token);
            
            token = strtok(NULL, "|");
            saldo = atof(token);
            
            token = strtok(NULL, "|");
            strcpy(ktp.nik, token);
            
            token = strtok(NULL, "|");
            strcpy(ktp.namaLengkap, token);
            
            token = strtok(NULL, "|");
            strcpy(ktp.alamat, token);
            
            token = strtok(NULL, "|");
            strcpy(ktp.tanggalLahir, token);
            
            users[idx] = User(username, password, pin, saldo);
            users[idx].setKTP(ktp);
            idx++;
        }
        
        file.close();
        return count;
    }
    
    bool loadAdmin(const char* username, const char* password, char* adminID) {
        ifstream file(adminFile);
        if (!file.is_open()) {
            return false;
        }
        
        char line[200];
        while (file.getline(line, 200)) {
            char user[50], pass[50], id[20];
            
            char* token = strtok(line, "|");
            strcpy(user, token);
            
            token = strtok(NULL, "|");
            strcpy(pass, token);
            
            token = strtok(NULL, "|");
            strcpy(id, token);
            
            if (strcmp(user, username) == 0 && strcmp(pass, password) == 0) {
                strcpy(adminID, id);
                file.close();
                return true;
            }
        }
        
        file.close();
        return false;
    }
    
    void loadTransactions(AVLTree& avlTree) {
        ifstream file(transactionFile);
        if (!file.is_open()) {
            return;
        }
        
        char line[500];
        while (file.getline(line, 500)) {
            Transaksi t;
            
            char* token = strtok(line, "|");
            strcpy(t.id, token);
            
            token = strtok(NULL, "|");
            strcpy(t.timestamp, token);
            
            token = strtok(NULL, "|");
            strcpy(t.jenis, token);
            
            token = strtok(NULL, "|");
            strcpy(t.dari, token);
            
            token = strtok(NULL, "|");
            strcpy(t.ke, token);
            
            token = strtok(NULL, "|");
            t.jumlah = atof(token);
            
            token = strtok(NULL, "|");
            if (token) strcpy(t.keterangan, token);
            
            avlTree.insert(t);
        }
        
        file.close();
    }
    
    void saveUsers(User* users, int count) {
        ofstream file(userFile);
        if (!file.is_open()) {
            cout << "Error: Tidak bisa menyimpan data users\n";
            return;
        }
        
        for (int i = 0; i < count; i++) {
            KTP ktp = users[i].getKTP();
            file << users[i].getUsername() << "|"
                 << users[i].getPassword() << "|"
                 << users[i].getPIN() << "|"
                 << users[i].getSaldo() << "|"
                 << ktp.nik << "|" << ktp.namaLengkap << "|"
                 << ktp.alamat << "|" << ktp.tanggalLahir << "\n";
        }
        
        file.close();
    }
    
    void appendTransaction(const Transaksi& t) {
        ofstream file(transactionFile, ios::app);
        if (!file.is_open()) {
            cout << "Error: Tidak bisa menyimpan transaksi\n";
            return;
        }
        
        file << t.id << "|" << t.timestamp << "|" << t.jenis << "|"
             << t.dari << "|" << t.ke << "|" << t.jumlah << "|"
             << t.keterangan << "\n";
        
        file.close();
    }
};

#endif
