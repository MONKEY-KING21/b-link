// Copyright
#ifndef SOURCE_B_LINK_HPP_
#define SOURCE_B_LINK_HPP_
#include <iostream>
#include <queue>
#include <condition_variable>
#include <deque>
#include <vector>
#include <mutex>
#include <shared_mutex>
#include <sstream>
#include <thread>
#include <queue>
#include <random>
#include <utility>
#define d 2 //min key
using namespace std;
std::size_t NUMBER_THREADS = 8;
const std::size_t NUMBER_OPERATIONS = 100000;
int MIN_VALUE = 1;
int MAX_VALUE = 15000;
std::random_device rd;

class TreeNode {
public:
	shared_mutex mutex;
	int cant_llave, cant_ptr;
	vector<int>key;
	TreeNode* izq;
	TreeNode* der;
	vector<TreeNode*>ptr;
	bool hoja;
	TreeNode(bool hoja) {
		cant_llave = 0;
		cant_ptr = 0;
		key.resize(2 * d + 1);
		ptr.resize(2 * d + 2, NULL);
		this->hoja = hoja;
		izq = der = NULL;}

	void insertar_k(const int& value) {
		int i;
		for (i = cant_llave; i >= 1; i--) {
			if (cant_llave != key.size()) {
				if (key[i - 1] > value) {
					key[i] = key[i - 1];
				}
				else{
					key[i] = value;
					break;}}
		}
		if (!i)
			key[i] = value;
		cant_llave++;
	}

	void insertar_p(TreeNode* t) {
		int i;
		for (i = cant_ptr; i >= 1; i--) {
			if (ptr[i - 1]->key[0] > t->key[0]) {
				ptr[i] = ptr[i - 1];}
			else {
				ptr[i] = t;
				break;}
		}
		if (!i)
			ptr[i] = t;
		cant_ptr++;
	}

	int borrar_k() {
		cant_llave--;
		return key[cant_llave];}

	int borrar_k1() {
		int frontKey = key[0];
		for (int i = 1; i < cant_llave; i++)
			key[i - 1] = key[i];
		cant_llave--;
		return frontKey;}
	TreeNode* borrar_p() {
		cant_ptr--;
		return ptr[cant_ptr];
	}

	bool borrar_p(TreeNode* t) {
		bool x = 0;
		for (int i = 0; i < cant_ptr; i++) {
			if (x) {
				ptr[i - 1] = ptr[i];
				continue;}
			if (ptr[i] == t) {
				x = 1;}
		}
		cant_ptr -= x;
		return x;
	}
	
	void hacer_e(TreeNode* t2) {
		t2->izq = this;
		t2->der = this->der;
		this->der = t2;
		if (t2->der)
			t2->der->izq = t2;
	}

	bool borrar_l(const int& t) {
		bool x = 0;
		for (int i = 0; i < cant_llave; i++) {
			if (x) {
				key[i - 1] = key[i];
				continue;}
			if (key[i] == t) {
				x = 1;}
		}
		cant_llave -= x;
		return x;
	}

	bool borrar_p(TreeNode* t) {
		bool x = 0;
		for (int i = 0; i < cant_ptr; i++) {
			if (x) {
				ptr[i - 1] = ptr[i];
				continue;}
			if (ptr[i] == t) {
				x = 1;
				free(ptr[i]);}
		}
		cant_ptr -= x;
		return x;
	}
	int ultima_l() { return key[cant_llave - 1]; }
	int primer_l() { return key[0]; }
};
//EL blink paralelo
namespace EDA {
	namespace Concurrent {
		template <std::size_t B, typename Type>
		class BLinkTree {
		public:
			typedef Type data_type;
			TreeNode* raiz;
			BLinkTree() {raiz = new TreeNode(true);}
			~BLinkTree() {}
			std::size_t size() const {}
			bool empty() const {}
			bool search(const data_type& value) const {
				return buscar1(value, raiz, NULL);
			}

			void insert(const data_type& value) {
				queue<TreeNode*> l1;
				TreeNode* child = insertar1(value, raiz, l1);
				if (raiz->hoja && child && child != raiz) {
					TreeNode* newRoot = new TreeNode(false);
					newRoot->mutex.lock();
					newRoot->insertar_k(child->value[0]);
					newRoot->insertar_p(raiz);
					newRoot->insertar_p(child);
					raiz = newRoot;
					newRoot->mutex.unlock();
				}
				desb2(l1);
			}

			void remove(const data_type& value) {}

			void imprimir() {
				queue<TreeNode*>l1;
				l1.push(raiz);
				while (!l1.empty()) {
					int s = l1.size();
					for (int i = 0; i < s; i++) {
						for (int i = 0; i < l1.front()->cant_llave; i++)
							cout << l1.front()->key[i] << " ";
						for (int i = 0; i < l1.front()->cant_ptr; i++)
							l1.push(l1.front()->ptr[i]);
						if (i != s - 1)
							cout << "  ,  ";
						l1.pop();
					}
					cout << endl;
				}
			}

		private:
			data_type* data_;
			void desb1(queue<TreeNode*>& l1) {
				for (int i = 0; i < l1.size(); i++) {
					l1.front()->mutex.unlock();
					l1.pop();}
			}
			void desb2(queue<TreeNode*>& l1) {
				while (!l1.empty()) {
					l1.front()->mutex.unlock();
					l1.pop();}
			}
			bool buscar1(const int& key, TreeNode* ptr, TreeNode* parent) {
				if (!parent) {
					ptr->mutex.lock_shared();}
				else {
					ptr->mutex.lock_shared();
					parent->mutex.unlock_shared();}
				if (ptr->hoja) {
					for (int i = 0; i < ptr->cant_llave; i++)
						if (ptr->key[i] == key) {
							ptr->mutex.unlock_shared();
							return true;}
					ptr->mutex.unlock_shared();
					return false;}
				if (!ptr->cant_llave) {
					ptr->mutex.unlock_shared();
					return false;}
				if (key < ptr->key[0])
					return buscar1(key, ptr->ptr[0], ptr);
				if (key > ptr->key[ptr->cant_llave])
					return buscar1(key, ptr->ptr[ptr->cant_llave], ptr);
				for (int i = 0; i < ptr->cant_llave - 1; i++)
					if (key >= ptr->key[i] && key < ptr->key[i + 1])
						return buscar1(key, ptr->ptr[i + 1], ptr);
				ptr->mutex.unlock_shared();
				return false;
			}

			TreeNode* insertar1(const int& key, TreeNode* ptr, queue<TreeNode*>& l1) {
				ptr->mutex.lock();
				l1.push(ptr);
				if (ptr->cant_llave <= 2 * d) {
					desb1(l1);}
				if (ptr->hoja) {
					ptr->insertar_k(key);
					if (ptr->cant_llave <= 2 * d) {
						return NULL;}
					TreeNode* t = new TreeNode(true);
					t->mutex.lock();
					for (int i = 0; i < d + 1; i++) {
						int k = ptr->borrar_k();
						t->insertar_k(k);
					}
					ptr->hacer_e(t);
					t->mutex.unlock();
					return t;
				}
				TreeNode* ptr1 = nullptr;
				if (key < ptr->key[0])
					ptr1 = insertar1(key, ptr->ptr[0], l1);
				else if (key >= ptr->key[ptr->cant_llave - 1])
					ptr1 = insertar1(key, ptr->ptr[ptr->cant_llave], l1);
				else
					for (int i = 0; i < ptr->cant_llave - 1; i++)
						if (key >= ptr->key[i] && key < ptr->key[i + 1]) {
							ptr1 = insertar1(key, ptr->ptr[i + 1], l1);
							break;}
				if (!ptr1) {
					return ptr1;}
				ptr->insertar_k(ptr1->key[0]);
				if (!ptr1->hoja) {
					ptr1->borrar_l(ptr1->primer_l());}
				ptr->insertar_p(ptr1);
				if (ptr->cant_llave <= 2 * d)
					return NULL;
				TreeNode* x = new TreeNode(false);
				x->mutex.lock();
				for (int i = 0; i < d + 1; i++) {
					x->insertar_k(ptr->borrar_k());
					x->insertar_p(ptr->borrar_p());}
				x->mutex.unlock();
				if (ptr == raiz) {
					TreeNode* newRoot = new TreeNode(false);
					newRoot->mutex.lock();
					newRoot->insertar_k(x->borrar_k1());
					newRoot->insertar_p(ptr);
					newRoot->insertar_p(x);
					raiz = newRoot;
					newRoot->mutex.unlock();
				}
				return x;
			}
		};

	}
}

#endif  // SOURCE_B_LINK_HPP_
