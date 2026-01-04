#ifndef USER_H
#define USER_H

#include <iostream>
#include <cstring>
#include <iomanip>

using namespace std;

// =================================================================
// STRUCT KTP - Data Identitas User
// =================================================================
struct KTP {
    char nik[17];           // NIK 16 digit + null terminator
    char namaLengkap[100];
    char alamat[200];
    char tanggalLahir[11];  // Format: DD/MM/YYYY
    
    KTP() {
        strcpy(nik, "");
        strcpy(namaLengkap, "");
        strcpy(alamat, "");
        strcpy(tanggalLahir, "");
    }
    
    // Validasi NIK harus 16 digit angka
    bool validasiNIK() const {
        if (strlen(nik) != 16) return false;
        for (int i = 0; i < 16; i++) {
            if (nik[i] < '0' || nik[i] > '9') return false;
        }
        return true;
    }
    
    void display() const {
        cout << "      NIK            : " << nik << "\n";
        cout << "      Nama Lengkap   : " << namaLengkap << "\n";
        cout << "      Alamat         : " << alamat << "\n";
        cout << "      Tanggal Lahir  : " << tanggalLahir << "\n";
    }
};

// =================================================================
// CLASS USER - User dengan Saldo, PIN, dan KTP
// =================================================================
/*
 * Class User merepresentasikan pengguna E-Wallet dengan fitur:
 * - Saldo untuk transaksi
 * - PIN 6 digit untuk keamanan
 * - Data KTP untuk identitas
 * - Fungsi transaksi (Top Up, Transfer, Pembayaran)
 */
class User {
private:
    char username[50];
    char password[50];
    double saldo;
    char pin[7];  // 6 digit + null terminator
    KTP dataKTP;
    
public:
    User() {
        strcpy(username, "");
        strcpy(password, "");
        saldo = 0.0;
        strcpy(pin, "");
    }
    
    User(const char* u, const char* p, const char* pn, double s = 0.0) {
        strcpy(username, u);
        strcpy(password, p);
        saldo = s;
        strcpy(pin, pn);
    }
    
    virtual ~User() {}
    
    void displayInfo() const {
        cout << "\n      === Informasi User ===\n";
        cout << "      Username       : " << username << "\n";
        cout << "      Saldo          : Rp " << fixed << setprecision(2) << saldo << "\n";
        cout << "\n      === Data KTP ===\n";
        dataKTP.display();
    }
    
    const char* getUsername() const { return username; }
    const char* getPassword() const { return password; }
    
    bool authenticate(const char* user, const char* pass) const {
        return (strcmp(username, user) == 0 && strcmp(password, pass) == 0);
    }
    
    void setPassword(const char* newPass) {
        strcpy(password, newPass);
    }
    
    double getSaldo() const { return saldo; }
    
    void addSaldo(double amount) {
        if (amount > 0) {
            saldo += amount;
        }
    }
    
    bool deductSaldo(double amount) {
        if (amount > 0 && saldo >= amount) {
            saldo -= amount;
            return true;
        }
        return false;
    }
    
    bool verifyPIN(const char* inputPin) const {
        return strcmp(pin, inputPin) == 0;
    }
    
    void setPIN(const char* newPin) {
        if (strlen(newPin) == 6) {
            strcpy(pin, newPin);
        }
    }
    
    void setKTP(const KTP& ktp) {
        dataKTP = ktp;
    }
    
    KTP getKTP() const {
        return dataKTP;
    }
    
    const char* getPIN() const {
        return pin;
    }
};

#endif
