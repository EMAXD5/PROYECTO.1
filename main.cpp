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