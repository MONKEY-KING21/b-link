// Copyright
#ifndef SOURCE_B_LINK_HPP_
#define SOURCE_B_LINK_HPP_
int MAX;
class BLinkTree;
#include <utility>
struct Node {
	int* llave;
	int* tamanio;
	bool hoja;
	Node** ptr;
	friend class BLinkTree;
public:
	Node() {
		llave = new int[MAX];
		ptr = new Node * [MAX + 1];
	}
};
namespace EDA {
	namespace Concurrent {

		template <std::size_t B, typename Type>
		class BLinkTree {
		public:
			typedef Type data_type;

			BLinkTree() {
				raiz = nullptr;
			}
			~BLinkTree() {}
			std::size_t size() const {}
			bool empty() const {}
			bool search(const data_type& value) const {
				if (raiz == NULL){
					cout << "el arbol esta vacio"<<endl;
				}
				else{
					Node* ptr1 = raiz;
					while (ptr1->hoja == false){
						for (int i = 0; i < ptr1->tamanio; i++){
							if (value < ptr1->llave[i]){
								ptr1 = ptr1->ptr[i];
								break;
							}
							if (i == ptr1->tamanio - 1){
								ptr1 = ptr1->ptr[i + 1];
								break;
							}
						}
					}
					for (int i = 0; i < ptr1->tamanio; i++){
						if (ptr1->llave[i] == value){
							cout <<"el dato esta"<<endl;
							return true;
						}
					}
					cout << "el dato no esta"<<endl;
					return false;
				}
			}
			void insert(const data_type& value) {
				if (raiz == NULL){
					raiz = new Node;
					raiz->llave[0] = value;
					raiz->hoja = true;
					raiz->tamanio = 1;
					cout << "raiz creada con el valor de : " << value <<endl;
				}
				else{
					Node* ptr1 = raiz;
					Node* padre;
					while (ptr1->hoja == false){
						padre = ptr1;
						for (int i = 0; i < ptr1->tamanio; i++){
							if (value < ptr1->llave[i]){
								ptr1 = ptr1->ptr[i];
								break;
							}
							if (i == ptr1->tamanio - 1){
								ptr1 = ptr1->ptr[i + 1];
								break;
							}
						}
					}
					if (ptr1->tamanio < MAX){
						int i = 0;
						while (value> ptr1->llave[i] && i < ptr1->tamanio) i++;
						for (int j = ptr1->tamanio; j > i; j--){
							ptr1->llave[j] = ptr1->llave[j - 1];
						}
						ptr1->llave[i] = value;
						ptr1->tamanio++;
						ptr1->ptr[ptr1->tamanio] = ptr1->ptr[ptr1->tamanio - 1];
						ptr1->ptr[ptr1->tamanio - 1] = NULL;
					}
					else{
						Node* nueva_hoja = new Node;
						int virtualNode[MAX + 1];
						for (int i = 0; i < MAX; i++){
							virtualNode[i] = ptr1->llave[i];
						}
						int i = 0, j;
						while (value > virtualNode[i] && i < MAX) i++;
						for (int j = MAX + 1; j > i; j--){
							virtualNode[j] = virtualNode[j - 1];
						}
						virtualNode[i] = value;
						nueva_hoja->hoja = true;
						ptr1->tamanio = (MAX + 1) / 2;
						nueva_hoja->tamanio = MAX + 1 - (MAX + 1) / 2;
						ptr1->ptr[ptr1->tamanio] = nueva_hoja;
						nueva_hoja->ptr[nueva_hoja->tamanio] = ptr1->ptr[MAX];
						ptr1->ptr[MAX] = NULL;
						for (i = 0; i < ptr1->tamanio; i++){
							ptr1->llave[i] = virtualNode[i];
						}
						for (i = 0, j = ptr1->tamanio; i < nueva_hoja->tamanio; i++, j++){
							nueva_hoja->llave[i] = virtualNode[j];
						}
						if (ptr1 == raiz){
							Node* nueva_raiz = new Node;
							nueva_raiz->llave[0] = nueva_hoja->llave[0];
							nueva_raiz->ptr[0] = ptr1;
							nueva_raiz->ptr[1] = nueva_hoja;
							nueva_raiz->hoja = false;
							nueva_raiz->tamanio = 1;
							raiz = nueva_raiz;
						}
						else{
							insertar2(nueva_hoja->llave[0], padre, nueva_hoja);
						}
					}
				}
			}
			void remove(const data_type& value) {}
		private:
			Node* raiz;
			Node* find_p(Node* ptr1, Node* ptr2){
				Node* padre;
				if (ptr1->hoja || (ptr1->ptr[0])->hoja){
					return NULL;
				}
				for (int i = 0; i < ptr1->tamanio + 1; i++){
					if (ptr1->ptr[i] == ptr2){
						padre = ptr1;
						return padre;
					}
					else{
						padre = find_p(ptr1->ptr[i], ptr2);
						if (padre != NULL)return padre;
					}
				}
				return padre;
			}
			void insertar2(int x, Node* ptr1, Node* ptr2){
				if (ptr1->tamanio < MAX){
					int i = 0;
					while (x > ptr1->llave[i] && i < ptr1->tamanio) i++;
					for (int j = ptr1->tamanio; j > i; j--){
						ptr1->llave[j] = ptr1->llave[j - 1];
					}
					for (int j = ptr1->tamanio + 1; j > i + 1; j--){
						ptr1->ptr[j] = ptr1->ptr[j - 1];
					}
					ptr1->llave[i] = x;
					ptr1->tamanio++;
					ptr1->ptr[i + 1] = ptr2;
				}
				else{
					Node* nuevo_inter = new Node;
					int virtualKey[MAX + 1];
					Node* virtualPtr[MAX + 2];
					for (int i = 0; i < MAX; i++){
						virtualKey[i] = ptr1->llave[i];
					}
					for (int i = 0; i < MAX + 1; i++){
						virtualPtr[i] = ptr1->ptr[i];
					}
					int i = 0, j;
					while (x > virtualKey[i] && i < MAX) i++;
					for (int j = MAX + 1; j > i; j--){
						virtualKey[j] = virtualKey[j - 1];
					}
					virtualKey[i] = x;
					for (int j = MAX + 2; j > i + 1; j--){
						virtualPtr[j] = virtualPtr[j - 1];
					}
					virtualPtr[i + 1] = ptr2;
					nuevo_inter->hoja = false;
					ptr1->tamanio = (MAX + 1) / 2;
					nuevo_inter->tamanio = MAX - (MAX + 1) / 2;
					for (i = 0, j = ptr1->tamanio + 1; i < nuevo_inter->tamanio; i++, j++){
						nuevo_inter->llave[i] = virtualKey[j];
					}
					for (i = 0, j = ptr1->tamanio + 1; i < nuevo_inter->tamanio + 1; i++, j++){
						nuevo_inter->ptr[i] = virtualPtr[j];
					}
					if (ptr1 == raiz){
						Node* nueva_raiz = new Node;
						nueva_raiz->llave[0] = ptr1->llave[ptr1->tamanio];
						nueva_raiz->ptr[0] = ptr1;
						nueva_raiz->ptr[1] = nuevo_inter;
						nueva_raiz->hoja = false;
						nueva_raiz->tamanio = 1;
						raiz = nueva_raiz;
					}
					else{
						insertar2(ptr1->llave[ptr1->tamanio], find_p(raiz, ptr1), nuevo_inter);
					}
				}
			}
		};

	}  // namespace Concurrent
}  // namespace EDA

#endif  // SOURCE_B_LINK_HPP_
