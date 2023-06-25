#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <locale>

using namespace std;

class Node { // класс представляет узел дерева
public:
	int id; // идентификатор узла
	int parent_id; // идентификатор родительского узла
	string name; // название узла
	string value; // значение узла
	vector<Node*> children; // вектор который хранит дочерние узлы.

	Node(int id, int parent_id, string name, string value) { // конструктор инициализирует свойства объекта
		this->id = id;
		this->parent_id = parent_id;
		this->name = name;
		this->value = value;
	}
};

class Tree { // Класс представляет дерево
public:
	vector<Node*> nodes; // вектор который хранит все узлы дерева

	void addNode(int id, int parent_id, string name, string value) { // Метод создает новый узел и добавляет его в вектор `nodes`
		Node* node = new Node(id, parent_id, name, value);
		nodes.push_back(node);

		if (parent_id != 0) {
			Node* parent = findNode(parent_id);
			parent->children.push_back(node);
		}
	}

	Node* findNode(int id) { // Метод находит узел по его идентификатору
		for (int i = 0; i < nodes.size(); i++) {
			if (nodes[i]->id == id) {
				return nodes[i];
			}
		}
		return nullptr;
	}

	void updateNode(int id, string value) { // Метод обновляет значение узла по его идентификатору
		Node* node = findNode(id);
		node->value = value;
	}
};

int id_counter = 0; // счетчик идентификаторов узлов
Tree tree;
void parse_node(string line, int parent_id) { // Метод для парсинга строки и создания узла
	if (line == "} ") { // если строка равна "} "
		for (size_t i = id_counter; i > 0; i--) {
			if (line == "} " && tree.findNode(i)->value == "") {
				tree.updateNode(i, "x"); // игнорируем узел
				break;
			}
		}
	}
	for (size_t i = id_counter; i > 0; i--) {
		if (tree.findNode(i)->value == "") { // если значение в узле равно ""
			parent_id = tree.findNode(i)->id; // ищем вышестоящего родителя
			break;
		}
	}
	if (line != "} ") { // если строка не равна "} "
		size_t pos = line.find("="); // ищем позицию знака "="
		string name = line.substr(0, pos); // выделяем имя узла
		name = name.substr(0, name.size() - 1); // удаляем пробелы|знаки вокруг имени
		regex pattern("^[a-zA-Z_][a-zA-Z0-9_]*$"); // Регулярное выражение для проверки строки
		if (regex_match(name, pattern) == false) { // Проверяем соответствие строки регулярному выражению
			cout << "Не верный формат строки: " << line << endl;
			exit(1); // завершаем работу программы с кодом ошибки
		}
		string value = line.substr(pos + 1); // выделяем значение узла
		value = value.substr(1, value.size() - 2); // удаляем пробелы|знаки вокруг значения
		if (value.front() == '{') { // если значение узла является вертикальным списком
			tree.addNode(++id_counter, parent_id, name, "");
		}
		else if (value.front() == '"' && value.back() == '"') { // если значение узла не является списком
			value = value.substr(1, value.size() - 2); // удаляем пробелы|знаки вокруг значения
			tree.addNode(++id_counter, parent_id, name, value);
		}
		int counter = 0; // создаем|обнуляем счетчик количества элементов в горизонтальном списке
		if (value.front() == '{' && value.back() == '}') { // если значение узла является списком
			value = value.substr(2, value.size() - 3); // удаляем скобки вокруг списка
			string line = value; // создаем временную строку
			size_t start_pos = 0; // начальная позиция для поиска дочерних узлов
			while (line.size() != NULL) { // пока не прошли весь список
				size_t pos = line.find("="); // ищем позицию знака "="
				size_t end_pos = line.find('"', pos + 4); // ищем позицию знака "
				string value = line.substr(0, end_pos + 2); // выделяем значение узла
				regex pattern("^[^\n]*$");// Регулярное выражение для проверки строки
				if (regex_match(value, pattern) == false) { // Проверяем соответствие строки регулярному выражению
					cout << "Не верный формат строки: " << line << endl;
					exit(1); // завершаем работу программы с кодом ошибки
				}
				parse_node(value, 0); // рекурсивно вызываем функцию для создания дочернего узла
				line = line.substr(end_pos + 2, line.size() - 4); // обрезаем строку до следующего имени узла
				counter++; // инкрементируем счетчик количества элементов в горизонтальном списке
			}
			tree.updateNode(id_counter - counter, "x"); // игнорируем узел
		}
	}
}

void printTree(Node* node, ofstream& output_file, int depth = 0) { // Метод для построения выходного файла
	for (int i = 0; i < depth; i++) {
		output_file << " ";
	}
	if (node->value == "" || node->value == "x") { // если узел является списком
		string str; // временная строка
		for (size_t i = 0; i < node->children.size(); i++)
			str += to_string(node->children[i]->id) + " "; // собираем вектор в строку
		output_file << node->id << ", " << node->parent_id << ", " << node->name << ", " << str << endl; // выводим узел в файл с списком
	}
	else { // иначе если узел не является списком
		output_file << node->id << ", " << node->parent_id << ", " << node->name << ", " << node->value << endl; // выводим узел в файл с строкой
	}
	for (int i = 0; i < node->children.size(); i++) {
		printTree(node->children[i], output_file, depth + 1);
	}
}

int main(int argc, char* argv[]) {
	setlocale(LC_ALL, "Russian");
	//if (argc != 3) { // если не указаны имена входного и выходного файлов
	//	cout << "Использование: " << argv[0] << " <input_filename> <output_filename>" << endl;
	//	return 1; // завершаем работу программы с кодом ошибки
	//}
	ifstream input_file("input_file.txt"); // открываем входной файл
	//if (!input_file.is_open()) { // если не удалось открыть файл
	//	cout << "Не удалось открыть файл \"" << argv[1] << "\"" << endl;
	//	return 1; // завершаем работу программы с кодом ошибки
	//}
	string line; // временная строка
	while (getline(input_file, line)) { // читаем файл построчно
		parse_node(line, 0); // парсим строку и создаем узел
	}
	input_file.close(); // закрываем входной файл
	ofstream output_file("output_file.txt"); // создаем выходной файл
	//if (!output_file.is_open()) { // если не удалось создать файл
	//	cout << "Не удалось создать файл \"" << argv[2] << "\"" << endl;
	//	return 1; // завершаем работу программы с кодом ошибки
	//}
	printTree(tree.findNode(1), output_file);
	output_file.close(); // закрываем выходной файл
	return 0; // завершаем работу программы
}
