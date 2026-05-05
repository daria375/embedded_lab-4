#include <Arduino.h>

struct Node {
    float data;
    Node *left, *right;
};

Node* insert(Node* node, float data) {
    if (node == NULL) {
        Node* temp = new Node;
        temp->data = data;
        temp->left = temp->right = NULL;
        return temp;
    }
    if (data < node->data) node->left = insert(node->left, data);
    else node->right = insert(node->right, data);
    return node;
}

void deleteTree(Node* node) {
    if (node == NULL) return;
    deleteTree(node->left);
    deleteTree(node->right);
    delete node;
}

template <typename T>
void quickSort(T arr[], int left, int right) {
    int i = left, j = right;
    T pivot = arr[(left + right) / 2];
    while (i <= j) {
        while (arr[i] < pivot) i++;
        while (arr[j] > pivot) j--;
        if (i <= j) {
            T tmp = arr[i];
            arr[i] = arr[j];
            arr[j] = tmp;
            i++; j--;
        }
    }
    if (left < j) quickSort(arr, left, j);
    if (i < right) quickSort(arr, i, right);
}

void setup() {
    Serial.begin(115200);
    delay(1000);
}

void loop() {
    int frequencies[] = {240, 160, 80, 40};
    int sizes[] = {50, 100, 500, 1000};
    const char* typeNames[] = {"int", "float", "double", "char"};

    for (int freq : frequencies) {
        setCpuFrequencyMhz(freq);
        Serial.printf("\n=== ЧАСТОТА: %d МГц ===\n", freq);
        Serial.println("Тип,Розмір,Алгоритм,Час(us),Пам'ять(bytes)");

        for (int size : sizes) {
            int* intArr = new int[size];
            for(int i=0; i<size; i++) intArr[i] = random(0, 10000);
            
            unsigned long start = micros();
            quickSort(intArr, 0, size - 1);
            unsigned long end = micros();
            
            Serial.printf("int,%d,QuickSort,%lu,%d\n", size, end - start, size * sizeof(int));
            delete[] intArr;

            Node* root = NULL;
            uint32_t startMem = ESP.getFreeHeap();
            start = micros();
            for(int i=0; i<size; i++) {
                root = insert(root, (float)random(0, 10000) / 100.0);
            }
            end = micros();
            uint32_t endMem = startMem - ESP.getFreeHeap();

            Serial.printf("float,%d,BinaryTree,%lu,%u\n", size, end - start, endMem);
            deleteTree(root);
            
            delay(100); 
        }
    }
    Serial.println("\nТЕСТУВАННЯ ЗАВЕРШЕНО.");
    while(1); 
}