#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

using namespace std;

struct Node { // структура для хранения узла
  int id; // идентификатор узла
  int parent_id; // идентификатор родительского узла
  string name; // имя узла
  string value; // значение узла
  vector<int> children_id; // идентификаторы дочерних узлов
};

map<int, Node> nodes; // словарь для хранения всех узлов

int id_counter = 0; // счетчик идентификаторов узлов

void parse_node(string line, int parent_id) { // функция для парсинга строки и создания узла
  if (line == "} ") { // если строка равна "} "
    for (size_t i = id_counter; i > 0; i--) {
      if (line == "} " && nodes[i].value == "") {
        nodes[i].value = 'x'; // игнорируем узел
        break;
      }
    }
  }
  for (size_t i = id_counter; i > 0; i--) {
    if (nodes[i].value == "") { // если значение в узле равно ""
      parent_id = nodes[i].id; // ищем вышестоящего родителя
      break;
    }
  }
  if (line != "} ") { // если строка не равна "} "
    size_t pos = line.find("="); // ищем позицию знака "="
    string name = line.substr(0, pos); // выделяем имя узла
    name = name.substr(0, name.size() - 1); // удаляем пробелы|знаки вокруг имени
    string value = line.substr(pos + 1); // выделяем значение узла
    value = value.substr(1, value.size() - 2); // удаляем пробелы|знаки вокруг значения

    if (value.front() == '{') { // если значение узла является вертикальным списком
      nodes[id_counter] = { ++id_counter, parent_id, name, "", {} }; // создаем узел
      nodes[parent_id].children_id.push_back(id_counter); // добавляем идентификатор узла в вектор
    }
    else if (value.front() == '"' && value.back() == '"') { // если значение узла не является списком
      value = value.substr(1, value.size() - 2); // удаляем пробелы|знаки вокруг значения
      nodes[id_counter] = { ++id_counter, parent_id, name, value, {} }; // создаем узел без дочерних узлов
      nodes[parent_id].children_id.push_back(id_counter); // добавляем идентификатор узла в вектор
    }
    int counter = 0; // создаем|обнуляем счетчик количества элементов в горизонтальном списке
    if (value.front() == '{' && value.back() == '}') { // если значение узла является списком
      value = value.substr(2, value.size() - 3); // удаляем скобки вокруг списка
      string line = value; // создаем временную строку
      size_t start_pos = 0; // начальная позиция для поиска дочерних узлов
      while (line.size() != NULL) { // пока не прошли весь список
        size_t pos = line.find("="); // ищем позицию знака "="
        size_t end_pos = line.find('"', pos + 4); // ищем позицию знака "
        string value = line.substr(0, end_pos + 2); // выделяем имя узла
        parse_node(value, 0); // рекурсивно вызываем функцию для создания дочернего узла
        line = line.substr(end_pos + 2, line.size() - 4); // обрезаем строку до следующего имени узла
        counter++; // инкрементируем счетчик количества элементов в горизонтальном списке
      }
      nodes[id_counter - counter].value = 'x'; // игнорируем узел
    }
  }
}

void print_node(int id, int level, ofstream& output_file) { // функция для построения выходного файла

	Node node = nodes[id]; // получаем узел по его идентификатору

	string indent = string(level * 2, ' '); // создаем строку с нужным количеством отступов
	string str; // временная строка
	if (nodes[id].value == "" || nodes[id].value == "x") { // если узел является списком
		for (size_t i = 0; i < nodes[id].children_id.size(); i++)
			str += to_string(nodes[id].children_id[i]) + " "; // собираем вектор в строку
		output_file << indent << node.id << ", " << node.parent_id << ", " << node.name << ", " << str << endl; // выводим узел в файл с списком
	}
	else { // иначе если узел не является списком
		output_file << indent << node.id << ", " << node.parent_id << ", " << node.name << ", " << node.value << endl; // выводим узел в файл с строкой
	}
	for (int child_id : node.children_id) { // проходим по всем дочерним узлам
		print_node(child_id, level + 1, output_file); // рекурсивно вызываем функцию для каждого дочернего узла
	}
}

int main(int argc, char* argv[]) {
	if (argc != 3) { // если не указаны имена входного и выходного файлов
		cout << "Использование: " << argv[0] << " <input_filename> <output_filename>" << endl;
		return 1; // завершаем работу программы с кодом ошибки
	}
	ifstream input_file("input_file.txt"); // открываем входной файл
	if (!input_file.is_open()) { // если не удалось открыть файл
		cout << "Не удалось открыть файл \"" << argv[1] << "\"" << endl;
		return 1; // завершаем работу программы с кодом ошибки
	}
	string line; // временная строка
	while (getline(input_file, line)) { // читаем файл построчно
		parse_node(line, 0); // парсим строку и создаем узел
	}
	input_file.close(); // закрываем входной файл
	ofstream output_file("output_file.txt"); // создаем выходной файл
	if (!output_file.is_open()) { // если не удалось создать файл
		cout << "Не удалось создать файл \"" << argv[2] << "\"" << endl;
		return 1; // завершаем работу программы с кодом ошибки
	}
	print_node(1, 0, output_file); // выводим корневой узел в файл
	output_file.close(); // закрываем выходной файл
	return 0; // завершаем работу программы
}
