#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include <iostream>
#include <cstring>
#include <ctime>
#include <iomanip>

using namespace std;

// =================================================================
// STRUCT TRANSAKSI
// =================================================================
struct Transaksi {
    char id[20];
    char timestamp[20];     // Format: DD/MM/YYYY HH:MM:SS
    char jenis[20];         // "Top Up", "Transfer", "Pembayaran"
    char dari[50];          // Username pengirim
    char ke[50];            // Username penerima / merchant
    double jumlah;
    char keterangan[200];
    
    Transaksi() {
        strcpy(id, "");
        strcpy(timestamp, "");
        strcpy(jenis, "");
        strcpy(dari, "");
        strcpy(ke, "");
        jumlah = 0.0;
        strcpy(keterangan, "");
    }
    
    Transaksi(const char* j, double jml, const char* ket, const char* dr = "", const char* k = "") {
        static int counter = 1000;
        sprintf(id, "TRX%d", counter++);
        
        // Generate timestamp
        time_t now = time(0);
        struct tm* t = localtime(&now);
        sprintf(timestamp, "%02d/%02d/%04d %02d:%02d:%02d",
                t->tm_mday, t->tm_mon + 1, t->tm_year + 1900,
                t->tm_hour, t->tm_min, t->tm_sec);
        
        strcpy(jenis, j);
        jumlah = jml;
        strcpy(keterangan, ket);
        strcpy(dari, dr);
        strcpy(ke, k);
    }
    
    void display() const {
        cout << "| " << setw(15) << left << jenis 
             << " | Rp " << setw(10) << right << jumlah 
             << " | " << setw(30) << left << keterangan << " |" << endl;
    }
    
    void displayDetail() const {
        cout << "\n      +----------------------------------------+\n";
        cout << "      | ID Transaksi : " << setw(22) << left << id << "|\n";
        cout << "      | Waktu        : " << setw(22) << left << timestamp << "|\n";
        cout << "      | Jenis        : " << setw(22) << left << jenis << "|\n";
        cout << "      | Dari         : " << setw(22) << left << dari << "|\n";
        cout << "      | Ke           : " << setw(22) << left << ke << "|\n";
        cout << "      | Jumlah       : Rp " << setw(19) << right << jumlah << "|\n";
        cout << "      | Keterangan   : " << setw(22) << left << keterangan << "|\n";
        cout << "      +----------------------------------------+\n";
    }
};

// =================================================================
// STACK - LIFO untuk Refund Request
// =================================================================
/*
 * IMPLEMENTASI STACK (Last In First Out)
 * Digunakan untuk: Refund Request dari User
 * 
 * KONSEP:
 * - User mengajukan refund -> Push ke Stack
 * - Admin memproses refund terakhir -> Pop dari Stack
 * - Refund terbaru diproses terlebih dahulu (LIFO)
 */
struct RefundRequest {
    char username[50];
    char transactionId[20];
    char alasan[200];
    double jumlah;
    char timestamp[20];
    char status[20];  // "Pending", "Approved", "Rejected"
    
    RefundRequest() {
        strcpy(username, "");
        strcpy(transactionId, "");
        strcpy(alasan, "");
        jumlah = 0.0;
        strcpy(timestamp, "");
        strcpy(status, "Pending");
    }
};

struct StackNode {
    RefundRequest data;
    StackNode* next;
    
    StackNode(const RefundRequest& req) : data(req), next(NULL) {}
};

class RefundStack {
private:
    StackNode* top;
    int size;
    
public:
    RefundStack() : top(NULL), size(0) {}
    
    ~RefundStack() {
        while (!isEmpty()) {
            pop();
        }
    }
    
    // Push - Tambah refund ke stack (LIFO)
    void push(const RefundRequest& req) {
        StackNode* newNode = new StackNode(req);
        newNode->next = top;
        top = newNode;
        size++;
    }
    
    // Pop - Ambil refund terakhir yang masuk
    RefundRequest pop() {
        if (isEmpty()) {
            RefundRequest empty;
            return empty;
        }
        
        StackNode* temp = top;
        RefundRequest data = temp->data;
        top = top->next;
        delete temp;
        size--;
        
        return data;
    }
    
    RefundRequest peek() const {
        if (isEmpty()) {
            RefundRequest empty;
            return empty;
        }
        return top->data;
    }
    
    bool isEmpty() const {
        return top == NULL;
    }
    
    int getSize() const {
        return size;
    }
    
    void displayAll() const {
        if (isEmpty()) {
            cout << "      Tidak ada refund request.\n";
            return;
        }
        
        cout << "\n      === Stack Refund Request (LIFO - Terbaru di Atas) ===\n";
        StackNode* current = top;
        int no = 1;
        
        while (current != NULL) {
            cout << "\n      [" << no++ << "] Refund Request:\n";
            cout << "          Username    : " << current->data.username << "\n";
            cout << "          Transaction : " << current->data.transactionId << "\n";
            cout << "          Jumlah      : Rp " << current->data.jumlah << "\n";
            cout << "          Alasan      : " << current->data.alasan << "\n";
            cout << "          Timestamp   : " << current->data.timestamp << "\n";
            cout << "          Status      : " << current->data.status << "\n";
            current = current->next;
        }
    }
};

// =================================================================
// QUEUE - FIFO untuk Customer Service (Data yang baru masuk)
// =================================================================
/*
 * IMPLEMENTASI QUEUE (First In First Out)
 * Digunakan untuk: Customer Service - Keluhan User
 * 
 * KONSEP:
 * - User kirim keluhan -> Enqueue ke Queue
 * - Admin proses keluhan BERDASARKAN URUTAN MASUK (FIFO)
 * - Keluhan yang masuk pertama diproses terlebih dahulu
 */
struct CSTicket {
    char username[50];
    char pesan[500];
    char timestamp[20];
    char status[20];  // "Open", "In Progress", "Resolved"
    
    CSTicket() {
        strcpy(username, "");
        strcpy(pesan, "");
        strcpy(timestamp, "");
        strcpy(status, "Open");
    }
};

struct QueueNode {
    CSTicket data;
    QueueNode* next;
    
    QueueNode(const CSTicket& ticket) : data(ticket), next(NULL) {}
};

class CSQueue {
private:
    QueueNode* front;
    QueueNode* rear;
    int size;
    
public:
    CSQueue() : front(NULL), rear(NULL), size(0) {}
    
    ~CSQueue() {
        while (!isEmpty()) {
            dequeue();
        }
    }
    
    bool isEmpty() const {
        return front == NULL;
    }
    
    // Enqueue - Tambah ticket ke belakang queue
    void enqueue(const CSTicket& ticket) {
        QueueNode* newNode = new QueueNode(ticket);
        
        if (isEmpty()) {
            front = rear = newNode;
        } else {
            rear->next = newNode;
            rear = newNode;
        }
        size++;
    }
    
    // Dequeue - Ambil ticket dari depan queue (FIFO)
    CSTicket dequeue() {
        if (isEmpty()) {
            CSTicket empty;
            return empty;
        }
        
        QueueNode* temp = front;
        CSTicket data = temp->data;
        front = front->next;
        
        if (front == NULL) {
            rear = NULL;
        }
        
        delete temp;
        size--;
        
        return data;
    }
    
    CSTicket peek() const {
        if (isEmpty()) {
            CSTicket empty;
            return empty;
        }
        return front->data;
    }
    
    int getSize() const {
        return size;
    }
    
    void displayAll() const {
        if (isEmpty()) {
            cout << "      Tidak ada tiket CS.\n";
            return;
        }
        
        cout << "\n      === Queue CS Ticket (FIFO - Yang Masuk Pertama di Depan) ===\n";
        QueueNode* current = front;
        int no = 1;
        
        while (current != NULL) {
            cout << "\n      [" << no++ << "] Tiket CS:\n";
            cout << "          Username    : " << current->data.username << "\n";
            cout << "          Pesan       : " << current->data.pesan << "\n";
            cout << "          Timestamp   : " << current->data.timestamp << "\n";
            cout << "          Status      : " << current->data.status << "\n";
            current = current->next;
        }
    }
};

// =================================================================
// HUFFMAN CODING - Kompresi Keterangan Transaksi
// =================================================================
/*
 * HUFFMAN CODING - Algoritma Kompresi Data
 * 
 * CARA KERJA:
 * 1. Hitung frekuensi setiap karakter dalam string
 * 2. Bangun Min Heap berdasarkan frekuensi
 * 3. Ekstrak 2 node terkecil, gabungkan jadi parent baru
 * 4. Ulangi sampai tersisa 1 node (root pohon Huffman)
 * 5. Traverse pohon: kiri=0, kanan=1 untuk generate kode
 * 
 * KEUNTUNGAN:
 * - Karakter dengan frekuensi tinggi dapat kode lebih pendek
 * - Menghemat storage untuk data teks repetitif
 * - Kompresi lossless (data tidak hilang)
 */
struct HuffmanNode {
    char karakter;
    int frekuensi;
    HuffmanNode *left, *right;
    
    HuffmanNode(char k, int f) : karakter(k), frekuensi(f), left(NULL), right(NULL) {}
};

class HuffmanCoding {
private:
    HuffmanNode* root;
    char kode[256][100];  // Kode Huffman untuk setiap karakter ASCII
    
    // Min Heap manual
    HuffmanNode* heap[256];
    int heapSize;
    
    void heapifyDown(int idx) {
        int smallest = idx;
        int left = 2 * idx + 1;
        int right = 2 * idx + 2;
        
        if (left < heapSize && heap[left]->frekuensi < heap[smallest]->frekuensi)
            smallest = left;
        if (right < heapSize && heap[right]->frekuensi < heap[smallest]->frekuensi)
            smallest = right;
        
        if (smallest != idx) {
            HuffmanNode* temp = heap[idx];
            heap[idx] = heap[smallest];
            heap[smallest] = temp;
            heapifyDown(smallest);
        }
    }
    
    void heapifyUp(int idx) {
        int parent = (idx - 1) / 2;
        if (idx > 0 && heap[idx]->frekuensi < heap[parent]->frekuensi) {
            HuffmanNode* temp = heap[idx];
            heap[idx] = heap[parent];
            heap[parent] = temp;
            heapifyUp(parent);
        }
    }
    
    void insertHeap(HuffmanNode* node) {
        heap[heapSize] = node;
        heapifyUp(heapSize);
        heapSize++;
    }
    
    HuffmanNode* extractMin() {
        if (heapSize == 0) return NULL;
        HuffmanNode* minNode = heap[0];
        heap[0] = heap[heapSize - 1];
        heapSize--;
        heapifyDown(0);
        return minNode;
    }
    
    // Generate kode Huffman: kiri=0, kanan=1
    void generateKode(HuffmanNode* node, char* code, int depth) {
        if (!node) return;
        
        // Leaf node - simpan kode
        if (!node->left && !node->right) {
            code[depth] = '\0';
            strcpy(kode[(unsigned char)node->karakter], code);
            return;
        }
        
        // Traverse kiri (tambah '0')
        if (node->left) {
            code[depth] = '0';
            generateKode(node->left, code, depth + 1);
        }
        
        // Traverse kanan (tambah '1')
        if (node->right) {
            code[depth] = '1';
            generateKode(node->right, code, depth + 1);
        }
    }
    
public:
    HuffmanCoding() : root(NULL), heapSize(0) {
        for (int i = 0; i < 256; i++) {
            kode[i][0] = '\0';
        }
    }
    
    void buildTree(const char* text) {
        // 1. Hitung frekuensi
        int frekuensi[256] = {0};
        for (int i = 0; text[i] != '\0'; i++) {
            frekuensi[(unsigned char)text[i]]++;
        }
        
        // 2. Masukkan ke heap
        heapSize = 0;
        for (int i = 0; i < 256; i++) {
            if (frekuensi[i] > 0) {
                insertHeap(new HuffmanNode((char)i, frekuensi[i]));
            }
        }
        
        // 3. Bangun pohon Huffman
        while (heapSize > 1) {
            HuffmanNode* left = extractMin();
            HuffmanNode* right = extractMin();
            
            HuffmanNode* parent = new HuffmanNode('\0', left->frekuensi + right->frekuensi);
            parent->left = left;
            parent->right = right;
            
            insertHeap(parent);
        }
        
        root = extractMin();
        
        // 4. Generate kode
        char code[100];
        generateKode(root, code, 0);
    }
    
    void encode(const char* text, char* output) {
        output[0] = '\0';
        for (int i = 0; text[i] != '\0'; i++) {
            strcat(output, kode[(unsigned char)text[i]]);
        }
    }
    
    void displayKode() const {
        cout << "\n      === Tabel Kode Huffman ===\n";
        for (int i = 0; i < 256; i++) {
            if (kode[i][0] != '\0') {
                if (i == ' ') {
                    cout << "      '[SPACE]' -> " << kode[i] << "\n";
                } else if (i >= 32 && i <= 126) {
                    cout << "      '" << (char)i << "' -> " << kode[i] << "\n";
                }
            }
        }
    }
    
    void calculateEfficiency(const char* original) const {
        int originalBits = strlen(original) * 8;
        
        char encoded[10000];
        strcpy(encoded, "");
        for (int i = 0; original[i] != '\0'; i++) {
            strcat(encoded, kode[(unsigned char)original[i]]);
        }
        int compressedBits = strlen(encoded);
        
        cout << "\n      === Efisiensi Kompresi Huffman ===\n";
        cout << "      String Original   : \"" << original << "\"\n";
        cout << "      Panjang String    : " << strlen(original) << " karakter\n";
        cout << "      Ukuran Original   : " << originalBits << " bits (ASCII 8-bit)\n";
        cout << "      Ukuran Compressed : " << compressedBits << " bits\n";
        cout << "      Rasio Kompresi    : " << fixed << setprecision(2) 
             << (100.0 * compressedBits / originalBits) << "%\n";
        cout << "      Penghematan       : " << (originalBits - compressedBits) << " bits\n";
        cout << "\n      PENJELASAN:\n";
        cout << "      - Setiap karakter ASCII = 8 bits\n";
        cout << "      - Huffman memberikan kode lebih pendek untuk karakter sering muncul\n";
        cout << "      - Total bits berkurang = efisiensi storage meningkat\n";
    }
};

// =================================================================
// AVL TREE - Self-Balancing BST (BONUS)
// =================================================================
/*
 * AVL TREE - Binary Search Tree yang Selalu Seimbang
 * 
 * KONSEP:
 * - Balance Factor (BF) = Height(Left) - Height(Right)
 * - AVL menjaga |BF| <= 1 untuk SETIAP node
 * - Jika tidak seimbang, lakukan rotasi
 * 
 * JENIS ROTASI:
 * 1. Left Rotation (LL)
 * 2. Right Rotation (RR)
 * 3. Left-Right Rotation (LR)
 * 4. Right-Left Rotation (RL)
 * 
 * KEUNTUNGAN:
 * - Pencarian O(log n) - DIJAMIN
 * - Tree tidak akan menjadi skewed
 */
struct AVLNode {
    Transaksi data;
    AVLNode* left;
    AVLNode* right;
    int height;
    
    AVLNode(const Transaksi& t) : data(t), left(NULL), right(NULL), height(1) {}
};

class AVLTree {
private:
    AVLNode* root;
    int nodeCount;
    int rotationCount;
    
    int getHeight(AVLNode* node) const {
        return node ? node->height : 0;
    }
    
    int getBalance(AVLNode* node) const {
        return node ? getHeight(node->left) - getHeight(node->right) : 0;
    }
    
    void updateHeight(AVLNode* node) {
        if (node) {
            int leftH = getHeight(node->left);
            int rightH = getHeight(node->right);
            node->height = 1 + (leftH > rightH ? leftH : rightH);
        }
    }
    
    // Rotasi Kanan
    AVLNode* rotateRight(AVLNode* y) {
        rotationCount++;
        AVLNode* x = y->left;
        AVLNode* T2 = x->right;
        
        x->right = y;
        y->left = T2;
        
        updateHeight(y);
        updateHeight(x);
        
        return x;
    }
    
    // Rotasi Kiri
    AVLNode* rotateLeft(AVLNode* x) {
        rotationCount++;
        AVLNode* y = x->right;
        AVLNode* T2 = y->left;
        
        y->left = x;
        x->right = T2;
        
        updateHeight(x);
        updateHeight(y);
        
        return y;
    }
    
    AVLNode* insertRec(AVLNode* node, const Transaksi& t) {
        // 1. BST insert
        if (node == NULL) {
            nodeCount++;
            return new AVLNode(t);
        }
        
        if (t.jumlah < node->data.jumlah) {
            node->left = insertRec(node->left, t);
        } else {
            node->right = insertRec(node->right, t);
        }
        
        // 2. Update height
        updateHeight(node);
        
        // 3. Balance tree
        int balance = getBalance(node);
        
        // Left Left
        if (balance > 1 && t.jumlah < node->left->data.jumlah) {
            return rotateRight(node);
        }
        
        // Right Right
        if (balance < -1 && t.jumlah >= node->right->data.jumlah) {
            return rotateLeft(node);
        }
        
        // Left Right
        if (balance > 1 && t.jumlah >= node->left->data.jumlah) {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }
        
        // Right Left
        if (balance < -1 && t.jumlah < node->right->data.jumlah) {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }
        
        return node;
    }
    
    void inorderRec(AVLNode* node) const {
        if (node == NULL) return;
        inorderRec(node->left);
        node->data.display();
        inorderRec(node->right);
    }
    
    void searchRangeRec(AVLNode* node, double min, double max) const {
        if (node == NULL) return;
        
        if (node->data.jumlah > min) {
            searchRangeRec(node->left, min, max);
        }
        
        if (node->data.jumlah >= min && node->data.jumlah <= max) {
            node->data.display();
        }
        
        if (node->data.jumlah < max) {
            searchRangeRec(node->right, min, max);
        }
    }
    
    void visualizeRec(AVLNode* node, int space, int height) const {
        if (node == NULL) return;
        
        space += height;
        visualizeRec(node->right, space, height);
        
        cout << endl;
        for (int i = height; i < space; i++) cout << " ";
        cout << "Rp" << node->data.jumlah << " [BF:" << getBalance(node) << "]" << endl;
        
        visualizeRec(node->left, space, height);
    }
    
public:
    AVLTree() : root(NULL), nodeCount(0), rotationCount(0) {}
    
    void insert(const Transaksi& t) {
        root = insertRec(root, t);
    }
    
    void displayInorder() const {
        if (root == NULL) {
            cout << "      Tree kosong.\n";
            return;
        }
        cout << "-----------------------------------------------------------------------" << endl;
        cout << "| Jenis Transaksi | Jumlah        | Keterangan                        |" << endl;
        cout << "-----------------------------------------------------------------------" << endl;
        inorderRec(root);
        cout << "-----------------------------------------------------------------------" << endl;
    }
    
    void searchByRange(double min, double max) const {
        if (root == NULL) {
            cout << "      Tree kosong.\n";
            return;
        }
        cout << "-----------------------------------------------------------------------" << endl;
        cout << "| Jenis Transaksi | Jumlah        | Keterangan                        |" << endl;
        cout << "-----------------------------------------------------------------------" << endl;
        searchRangeRec(root, min, max);
        cout << "-----------------------------------------------------------------------" << endl;
    }
    
    void visualize() const {
        if (root == NULL) {
            cout << "      Tree kosong.\n";
            return;
        }
        cout << "\n      Struktur AVL Tree (Balance Factor ditampilkan):\n";
        visualizeRec(root, 0, 8);
    }
    
    void displayInfo() const {
        cout << "\n      === Informasi AVL Tree ===\n";
        cout << "      Total Node         : " << nodeCount << " transaksi\n";
        cout << "      Tinggi Tree        : " << getHeight(root) << " level\n";
        cout << "      Total Rotasi       : " << rotationCount << " kali\n";
        cout << "      Balance Factor Root: " << getBalance(root) << "\n";
        cout << "\n      KONSEP AVL TREE:\n";
        cout << "      - Balance Factor (BF) = Height(Left) - Height(Right)\n";
        cout << "      - |BF| <= 1 untuk SEMUA node (dijamin)\n";
        cout << "      - Pencarian: O(log n) - SELALU efisien!\n";
    }
    
    bool isEmpty() const {
        return root == NULL;
    }
};

#endif
