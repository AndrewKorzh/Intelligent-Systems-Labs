import json


def load_network_from_file(file_path):
    with open(file_path, "r") as file:
        # Читаем файл и загружаем JSON-данные
        network_data = json.load(file)

    structure = network_data["Structure"]
    neurons = [list(map(float, layer)) for layer in network_data["Neurons"]]
    weights = []
    for i in range(len(network_data["Weights"])):
        layer_weights = []
        for weight in network_data["Weights"][f"Layer {i}"]:
            layer_weights.append([float(w) for w in weight])
        weights.append(layer_weights)

    biases = []
    for i in range(len(network_data["Biases"])):
        biases.append([float(b) for b in network_data["Biases"][f"Layer {i}"]])

    return structure, neurons, weights, biases


import numpy as np


class NeuralNetwork:
    # Пример использования
    def __init__(self, file_path="network.json"):
        structure, neurons, weights, biases = load_network_from_file(file_path)
        self.structure = structure
        self.neurons = neurons
        self.weights = weights
        self.biases = biases

    def sigmoid(self, x):
        return 1 / (1 + np.exp(-x))

    def compute(self, input):
        # Копируем входные данные в первый слой нейронов
        self.neurons[0] = input

        # Процесс прохождения через слои
        for layer in range(len(self.weights)):
            for neuron in range(len(self.neurons[layer + 1])):
                # Инициализируем сумму с учётом смещения
                sum = self.biases[layer][neuron]

                # Складываем взвешенные суммы входных нейронов
                for prev_neuron in range(len(self.neurons[layer])):
                    sum += (
                        self.neurons[layer][prev_neuron]
                        * self.weights[layer][neuron][prev_neuron]
                    )

                # Применяем сигмоиду к сумме и сохраняем в нейрон текущего слоя
                self.neurons[layer + 1][neuron] = self.sigmoid(sum)

        # Возвращаем результат из последнего слоя
        return self.neurons[-1]


def generate_random_array(size=400, low=0, high=255):
    """
    Генерирует массив случайных целых чисел в диапазоне от low до high (включительно).

    :param size: количество элементов в массиве (по умолчанию 800)
    :param low: минимальное значение (по умолчанию 0)
    :param high: максимальное значение (по умолчанию 255)
    :return: массив случайных чисел
    """
    return np.random.randint(low, high + 1, size)


if __name__ == "__main__":
    network = NeuralNetwork()
    # Входные данные для сети
    input_data = generate_random_array()
    output = network.compute(input_data)
    print("Network output:", output)
