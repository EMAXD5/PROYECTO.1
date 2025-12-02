// proyecto.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <stdexcept>
#include <cctype>

using namespace std;

/* =========================
   MODELO DE DATOS
   ========================= */
struct Contact {
    string nombre;
    string telefono;
    string correo;

    Contact() {}
    Contact(string n, string t, string c) : nombre(move(n)), telefono(move(t)), correo(move(c)) {}
};

static string toLower(const string& s) {
    string t = s;
    for (char& c : t) c = (char)tolower((unsigned char)c);
    return t;
}

/* =========================
   ARREGLO DINÁMICO (ArrayList)
   ========================= */
class DynamicArray {
    Contact* data;
    size_t _size;
    size_t _cap;

    void grow() {
        _cap = max((size_t)1, _cap * 2);
        Contact* nuevo = new Contact[_cap];
        for (size_t i = 0; i < _size; i++) nuevo[i] = data[i];
        delete[] data;
        data = nuevo;
    }
public:
    DynamicArray() : data(nullptr), _size(0), _cap(0) {}
    ~DynamicArray() { delete[] data; }

    size_t size() const { return _size; }
    bool empty() const { return _size == 0; }

    const Contact& operator[](size_t i) const { return data[i]; }
    Contact& operator[](size_t i) { return data[i]; }

    void push_back(const Contact& c) {
        if (_size == _cap) grow();
        data[_size++] = c;
    }

    // elimina en posición i y compacta
    Contact removeAt(size_t i) {
        if (i >= _size) throw runtime_error("Indice invalido");
        Contact removed = data[i];
        for (size_t j = i + 1; j < _size; j++) data[j - 1] = data[j];
        _size--;
        return removed;
    }

    // ORDENAMIENTOS
    void sortByName() {
        sort(data, data + _size, [](const Contact& a, const Contact& b) {
            return toLower(a.nombre) < toLower(b.nombre);
            });
    }
    void sortByPhone() {
        sort(data, data + _size, [](const Contact& a, const Contact& b) {
            return a.telefono < b.telefono;
            });
    }

    // BÚSQUEDA SECUENCIAL
    vector<size_t> linearSearchByName(const string& name) {
        vector<size_t> idx;
        string key = toLower(name);
        for (size_t i = 0; i < _size; i++) {
            if (toLower(data[i].nombre).find(key) != string::npos) idx.push_back(i);
        }
        return idx;
    }
    vector<size_t> linearSearchByPhone(const string& phone) {
        vector<size_t> idx;
        for (size_t i = 0; i < _size; i++) {
            if (data[i].telefono.find(phone) != string::npos) idx.push_back(i);
        }
        return idx;
    }

    // BÚSQUEDA BINARIA (requiere orden)
    int binarySearchByName(const string& name) { // devuelve índice o -1
        // Asegúrate de haber llamado sortByName() antes de usarla
        string key = toLower(name);
        int l = 0, r = (int)_size - 1;
        while (l <= r) {
            int m = (l + r) / 2;
            string mid = toLower(data[m].nombre);
            if (mid == key) return m;
            if (mid < key) l = m + 1; else r = m - 1;
        }
        return -1;
    }
    int binarySearchByPhone(const string& phone) {
        // Depende de sortByPhone() previamente
        int l = 0, r = (int)_size - 1;
        while (l <= r) {
            int m = (l + r) / 2;
            if (data[m].telefono == phone) return m;
            if (data[m].telefono < phone) l = m + 1;
            else r = m - 1;
        }
        return -1;
    }
};

/* =========================
   LISTA ENLAZADA (registro de eliminados)
   ========================= */
struct NodeLL {
    Contact c;
    NodeLL* next;
    NodeLL(const Contact& _c) : c(_c), next(nullptr) {}
};
class LinkedList {
    NodeLL* head = nullptr;
public:
    ~LinkedList() { clear(); }
    void push_front(const Contact& c) {
        NodeLL* n = new NodeLL(c);
        n->next = head;
        head = n;
    }
    bool empty() const { return head == nullptr; }
    void print() {
        if (!head) { cout << " (vacio)\n"; return; }
        int i = 1;
        for (NodeLL* p = head; p; p = p->next) {
            cout << i++ << ") " << p->c.nombre << " | " << p->c.telefono
                << " | " << p->c.correo << "\n";
        }
    }
    void clear() {
        NodeLL* p = head;
        while (p) { NodeLL* q = p->next; delete p; p = q; }
        head = nullptr;
    }
};

/* =========================
   PILA (historial)
   ========================= */
class StackStr {
    vector<string> v;
public:
    void push(const string& s) { v.push_back(s); }
    bool empty() const { return v.empty(); }
    void printLIFO() {
        if (v.empty()) { cout << " (sin operaciones)\n"; return; }
        for (int i = (int)v.size() - 1; i >= 0; --i) cout << v[i] << "\n";
    }
};

/* =========================
   COLA (pendientes por agregar)
   ========================= */
struct NodeQ {
    Contact c; NodeQ* next;
    NodeQ(const Contact& _c) : c(_c), next(nullptr) {}
};
class QueueContacts {
    NodeQ* front_ = nullptr, * back_ = nullptr;
public:
    ~QueueContacts() { clear(); }
    void enqueue(const Contact& c) {
        NodeQ* n = new NodeQ(c);
        if (back_) back_->next = n;
        else front_ = n;
        back_ = n;
    }
    bool empty() const { return front_ == nullptr; }
    Contact dequeue() {
        if (!front_) throw runtime_error("Cola vacia");
        NodeQ* n = front_;
        Contact c = n->c;
        front_ = front_->next;
        if (!front_) back_ = nullptr;
        delete n;
        return c;
    }
    void print() {
        if (empty()) { cout << " (cola vacia)\n"; return; }
        int i = 1;
        for (NodeQ* p = front_; p; p = p->next) {
            cout << i++ << ") " << p->c.nombre << " | " << p->c.telefono
                << " | " << p->c.correo << "\n";
        }
    }
    void clear() { while (!empty()) dequeue(); }
};

/* =========================
   ÁRBOL BINARIO DE BÚSQUEDA (por nombre)
   ========================= */
struct BSTNode {
    Contact c;
    BSTNode* left, * right;
    BSTNode(const Contact& _c) : c(_c), left(nullptr), right(nullptr) {}
};
class BST {
    BSTNode* root = nullptr;

    void insert(BSTNode*& node, const Contact& c) {
        if (!node) { node = new BSTNode(c); return; }
        if (toLower(c.nombre) < toLower(node->c.nombre)) insert(node->left, c);
        else insert(node->right, c);
    }
    void inOrder(BSTNode* n) {
        if (!n) return;
        inOrder(n->left);
        cout << n->c.nombre << " | " << n->c.telefono << " | " << n->c.correo << "\n";
        inOrder(n->right);
    }
    void preOrder(BSTNode* n) {
        if (!n) return;
        cout << n->c.nombre << " | " << n->c.telefono << " | " << n->c.correo << "\n";
        preOrder(n->left);
        preOrder(n->right);
    }
    void clear(BSTNode* n) {
        if (!n) return;
        clear(n->left); clear(n->right); delete n;
    }
public:
    ~BST() { clear(root); }
    void rebuildFrom(const DynamicArray& arr) {
        clear(root); root = nullptr;
        for (size_t i = 0; i < arr.size(); ++i) insert(root, arr[i]);
    }
    void showInOrder() { if (!root) cout << " (arbol vacio)\n"; else inOrder(root); }
    void showPreOrder() { if (!root) cout << " (arbol vacio)\n"; else preOrder(root); }
};

/* =========================
   UTILIDADES DE ENTRADA
   ========================= */
string readLineTrim(const string& prompt) {
    cout << prompt;
    string s; getline(cin, s);
    // recortar espacios extremos
    auto l = s.find_first_not_of(" \t\r\n");
    auto r = s.find_last_not_of(" \t\r\n");
    if (l == string::npos) return "";
    return s.substr(l, r - l + 1);
}
Contact leerContactoInteractivo() {
    string n = readLineTrim("Nombre: ");
    string t = readLineTrim("Telefono: ");
    string c = readLineTrim("Correo: ");
    return Contact(n, t, c);
}
void printContactRow(const Contact& c, int i) {
    cout << setw(3) << i << ") " << left
        << setw(25) << c.nombre << " | "
        << setw(12) << c.telefono << " | "
        << c.correo << "\n";
}
void printList(const DynamicArray& arr) {
    cout << "---------------------------------------------\n";
    cout << "   Nombre                    | Telefono     | Correo\n";
    cout << "---------------------------------------------\n";
    for (size_t i = 0; i < arr.size(); ++i) printContactRow(arr[i], (int)i + 1);
    if (arr.size() == 0) cout << "(sin contactos)\n";
}

/* =========================
   APLICACIÓN (MENÚ)
   ========================= */
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    DynamicArray agenda;         // almacenamiento principal
    LinkedList eliminados;       // registro mediante lista enlazada
    StackStr historial;          // pila LIFO de operaciones
    QueueContacts pendientes;    // cola de contactos por agregar
    BST arbol;                   // BST por nombre

    while (true) {
        cout << "\n=========== GESTOR DE AGENDA (C++) ===========\n";
        cout << "1) Encolar nuevo contacto (cola de pendientes)\n";
        cout << "2) Transferir pendientes al arreglo principal\n";
        cout << "3) Listar contactos (ordenar por nombre/telefono)\n";
        cout << "4) Buscar contacto (secuencial o binaria)\n";
        cout << "5) Eliminar contacto (por indice / nombre)\n";
        cout << "6) Ver historial (pila)\n";
        cout << "7) Ver cola de pendientes\n";
        cout << "8) Mostrar arbol de contactos (inorden/preorden)\n";
        cout << "9) Ver lista enlazada de eliminados\n";
        cout << "0) Salir\n";
        cout << "Seleccione: ";
        string opt; getline(cin, opt);
        if (opt == "0" || cin.fail()) break;

        try {
            if (opt == "1") {
                cout << "\n-- Nuevo contacto (se encola como pendiente) --\n";
                Contact c = leerContactoInteractivo();
                pendientes.enqueue(c);
                cout << "Contacto encolado.\n";
            }
            else if (opt == "2") {
                cout << "\n-- Transfiriendo pendientes --> arreglo principal --\n";
                int cnt = 0;
                while (!pendientes.empty()) {
                    Contact c = pendientes.dequeue();
                    agenda.push_back(c);
                    historial.push("ADD: " + c.nombre);
                    cnt++;
                }
                cout << "Transferidos: " << cnt << ".\n";
                arbol.rebuildFrom(agenda);
            }
            else if (opt == "3") {
                cout << "\nOrdenar por: 1) Nombre  2) Telefono : ";
                string s; getline(cin, s);
                if (s == "1") { agenda.sortByName(); }
                else { agenda.sortByPhone(); }
                printList(agenda);
            }
            else if (opt == "4") {
                cout << "\nBuscar por: 1) Nombre  2) Telefono : ";
                string s; getline(cin, s);
                if (s == "1") {
                    string key = readLineTrim("Clave (nombre): ");
                    cout << "Metodo: 1) Secuencial  2) Binaria : ";
                    string m; getline(cin, m);
                    if (m == "2") {
                        agenda.sortByName(); // requisito para binaria
                        int pos = agenda.binarySearchByName(key);
                        if (pos == -1) cout << "No encontrado (binaria)\n";
                        else {
                            cout << "Encontrado (pos " << pos + 1 << "):\n";
                            printContactRow(agenda[pos], pos + 1);
                        }
                    }
                    else {
                        auto idx = agenda.linearSearchByName(key);
                        if (idx.empty()) cout << "No hay coincidencias.\n";
                        else {
                            cout << "Coincidencias:\n";
                            for (size_t k = 0; k < idx.size(); ++k)
                                printContactRow(agenda[idx[k]], (int)idx[k] + 1);
                        }
                    }
                }
                else {
                    string key = readLineTrim("Clave (telefono): ");
                    cout << "Metodo: 1) Secuencial  2) Binaria : ";
                    string m; getline(cin, m);
                    if (m == "2") {
                        agenda.sortByPhone();
                        int pos = agenda.binarySearchByPhone(key);
                        if (pos == -1) cout << "No encontrado (binaria)\n";
                        else {
                            cout << "Encontrado (pos " << pos + 1 << "):\n";
                            printContactRow(agenda[pos], pos + 1);
                        }
                    }
                    else {
                        auto idx = agenda.linearSearchByPhone(key);
                        if (idx.empty()) cout << "No hay coincidencias.\n";
                        else {
                            cout << "Coincidencias:\n";
                            for (size_t k = 0; k < idx.size(); ++k)
                                printContactRow(agenda[idx[k]], (int)idx[k] + 1);
                        }
                    }
                }
            }
            else if (opt == "5") {
                cout << "\nEliminar por: 1) Indice  2) Nombre exacto : ";
                string s; getline(cin, s);
                if (agenda.empty()) { cout << "Agenda vacia.\n"; continue; }

                if (s == "1") {
                    printList(agenda);
                    string idxs = readLineTrim("Indice a eliminar: ");
                    int i = stoi(idxs);
                    if (i<1 || i>(int)agenda.size()) { cout << "Indice invalido.\n"; continue; }
                    Contact rem = agenda.removeAt((size_t)(i - 1));
                    eliminados.push_front(rem);                 // usamos lista enlazada
                    historial.push("DEL: " + rem.nombre);       // pila
                    cout << "Eliminado: " << rem.nombre << "\n";
                }
                else {
                    string key = readLineTrim("Nombre exacto: ");
                    agenda.sortByName();
                    int pos = agenda.binarySearchByName(key);
                    if (pos == -1) { cout << "No existe \"" << key << "\".\n"; }
                    else {
                        Contact rem = agenda.removeAt((size_t)pos);
                        eliminados.push_front(rem);
                        historial.push("DEL: " + rem.nombre);
                        cout << "Eliminado: " << rem.nombre << "\n";
                    }
                }
                arbol.rebuildFrom(agenda);
            }
            else if (opt == "6") {
                cout << "\n-- Historial (pila, ultimo primero) --\n";
                historial.printLIFO();
            }
            else if (opt == "7") {
                cout << "\n-- Cola de pendientes --\n";
                pendientes.print();
            }
            else if (opt == "8") {
                cout << "\nRecorrido: 1) Inorden  2) Preorden : ";
                string s; getline(cin, s);
                arbol.rebuildFrom(agenda); // por si hubo cambios
                if (s == "2") arbol.showPreOrder();
                else arbol.showInOrder();
            }
            else if (opt == "9") {
                cout << "\n-- Lista enlazada de eliminados (cabeza = ultimo eliminado) --\n";
                eliminados.print();
            }
            else {
                cout << "Opcion invalida.\n";
            }
        }
        catch (const exception& e) {
            cout << "Error: " << e.what() << "\n";
        }
    }

    cout << "Saliendo...\n";
    return 0;
}
