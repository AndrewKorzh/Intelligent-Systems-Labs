using System;
using System.Diagnostics;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Threading.Tasks;

namespace NeuralNetwork1
{
    public class StudentNetwork : BaseNetwork
    {
        private int[] structure;
        private double[][] neurons;
        private double[][][] weights;
        private double[][] biases;
        private double learningRate = 0.15;

        private double[][] neuronsCounter;
        private Random random;

        public Stopwatch stopWatch = new Stopwatch();

        public StudentNetwork(int[] structure)
        {
            
            this.structure = structure;
            neuronsCounter = new double[structure.Length][];
            neurons = new double[structure.Length][];
            weights = new double[structure.Length - 1][][];
            biases = new double[structure.Length - 1][];
            random = new Random();

            for (int i = 0; i < structure.Length; i++) {
                Console.WriteLine(structure[i]);
                neurons[i] = new double[structure[i]];
                neuronsCounter[i] = new double[structure[i]];
            }


            for (int i = 0; i < structure.Length - 1; i++)
            {
                int currentLayerSize = structure[i];
                int nextLayerSize = structure[i + 1];

                weights[i] = new double[nextLayerSize][];
                biases[i] = new double[nextLayerSize];

                for (int j = 0; j < nextLayerSize; j++)
                {
                    weights[i][j] = new double[currentLayerSize];
                    for (int k = 0; k < currentLayerSize; k++)
                        weights[i][j][k] = random.NextDouble() * 2 - 1;
                    biases[i][j] = random.NextDouble() * 2 - 1;
                }
            }
        }

        public void SaveNetworkToFile(string filePath)
        {
            using (StreamWriter writer = new StreamWriter(filePath))
            {
                // Сохраняем структуру сети
                writer.WriteLine("{ \"Structure\": [");
                writer.WriteLine($"  {string.Join(",", structure)}");
                writer.WriteLine("],");

                // Сохраняем нейроны
                writer.WriteLine("  \"Neurons\": [");
                for (int i = 0; i < neurons.Length; i++)
                {
                    writer.WriteLine($"    [{string.Join(",", neurons[i].Select(n => FormatNumber(n)).ToArray())}]");
                    if (i < neurons.Length - 1)
                        writer.WriteLine(",");
                }
                writer.WriteLine("],");

                // Сохраняем веса
                writer.WriteLine("  \"Weights\": {");
                for (int i = 0; i < weights.Length; i++)
                {
                    writer.WriteLine($"    \"Layer {i}\": [");
                    for (int j = 0; j < weights[i].Length; j++)
                    {
                        writer.WriteLine($"      [{string.Join(",", weights[i][j].Select(w => FormatNumber(w)).ToArray())}]");
                        if (j < weights[i].Length - 1)
                            writer.WriteLine(",");
                    }
                    writer.WriteLine("    ]");
                    if (i < weights.Length - 1)
                        writer.WriteLine(",");
                }
                writer.WriteLine("  },");

                // Сохраняем смещения
                writer.WriteLine("  \"Biases\": {");
                for (int i = 0; i < biases.Length; i++)
                {
                    writer.WriteLine($"    \"Layer {i}\": [{string.Join(",", biases[i].Select(b => FormatNumber(b)).ToArray())}]");
                    if (i < biases.Length - 1)
                        writer.WriteLine(",");
                }
                writer.WriteLine("  }");

                writer.WriteLine("}");
            }
        }
        private string FormatNumber(double number)
        {
            // Если число слишком маленькое или близко к нулю, не выводить его с множеством знаков после запятой
            if (Math.Abs(number) < 1e-10)
                return "0"; // Возвращаем 0, если число очень маленькое
            else
                return number.ToString("0.##########", CultureInfo.InvariantCulture); // Форматируем с точностью до 10 знаков, используя точку
        }
        public override int Train(Sample sample, double acceptableError, bool parallel)
        {
            double error;
            int iters = 0;

            Console.WriteLine(sample);
            do
            {
                error = TrainSample(sample.input, sample.Output);
                iters++;
            } while (error > acceptableError);

            return iters;
        }

        public void ResetNeuronsCounter()
        {
            for (int i = 0; i < neuronsCounter.Length; i++)
            {
                for (int j = 0; j < neuronsCounter[i].Length; j++)
                {
                    neuronsCounter[i][j] = 0;
                }
            }
        }
        public override double TrainOnDataSet(SamplesSet samplesSet, int epochsCount, double acceptableError, bool parallel)
        {
            double totalError = double.PositiveInfinity;

            ResetNeuronsCounter();

            stopWatch.Restart();

            for (int epoch = 0; epoch < epochsCount; epoch++)
            {
                totalError = 0;

                foreach (Sample sample in samplesSet.samples)
                    totalError += TrainSample(sample.input, sample.Output);

                totalError /= samplesSet.Count;

                if (totalError <= acceptableError)
                    break;

                OnTrainProgress((double)epoch / epochsCount, totalError, stopWatch.Elapsed);
            }

            OnTrainProgress(1.0, totalError, stopWatch.Elapsed);

            stopWatch.Stop();

            for (int i = 1; i < neuronsCounter.Length; i++)
            {
                for (int j = 0; j < neuronsCounter[i].Length; j++)
                { // Исправлено условие
                    Console.Write(neuronsCounter[i][j] + " ");
                }
                Console.WriteLine("\n");
            }


            SaveNetworkToFile("C:\\Programming\\GIT_HUB\\Intelligent-Systems-Labs\\lab9\\network.json");

            return totalError;
        }

        protected override double[] Compute(double[] input)
        {
            //for (int i = 0; i < input.Length; i++) {
            //    Console.WriteLine(i +": " + input[i]);
            //}
             Array.Copy(input, neurons[0], input.Length);

            for (int layer = 0; layer < weights.Length; layer++)
            {
                Parallel.For(0, neurons[layer + 1].Length, neuron =>
                {
                    double sum = biases[layer][neuron];

                    for(int prevNeuron = 0; prevNeuron < neurons[layer].Length; prevNeuron++)
                    {
                        sum += neurons[layer][prevNeuron] * weights[layer][neuron][prevNeuron];
                    }
                    if (sum >= 5) {
                        neuronsCounter[layer + 1][neuron] += 1;
                    }
                    else {
                        neuronsCounter[layer + 1][neuron] = 0;
                    }
                    neurons[layer + 1][neuron] = Sigmoid(sum);
                });
            }

            return neurons[neurons.Length - 1];
        }

        private double TrainSample(double[] inputs, double[] expectedOutputs)
        {
            var outputs = Compute(inputs);

            double totalError = 0;
            double[][] errors = new double[structure.Length][];
            for (int i = 0; i < structure.Length; i++)
                errors[i] = new double[structure[i]];

            Parallel.For(0, expectedOutputs.Length, i =>
            {
                errors[errors.Length - 1][i] = expectedOutputs[i] - outputs[i];
                totalError += Math.Pow(errors[errors.Length - 1][i], 2);
            });
            totalError /= 2;

            for (int layer = weights.Length - 1; layer >= 0; layer--)
            {
                Parallel.For(0, weights[layer].Length, neuron =>
                {
                    double delta = 0;

                    if (layer == weights.Length - 1)
                        delta = (expectedOutputs[neuron] - neurons[layer + 1][neuron]) * SigmoidDerivative(neurons[layer + 1][neuron]);
                    else
                        delta = errors[layer + 1][neuron] * SigmoidDerivative(neurons[layer + 1][neuron]);

                    for(int prevNeuron = 0; prevNeuron < neurons[layer].Length; prevNeuron++)
                    {
                        weights[layer][neuron][prevNeuron] += learningRate * delta * neurons[layer][prevNeuron];
                        errors[layer][prevNeuron] += delta * weights[layer][neuron][prevNeuron];
                    }

                    biases[layer][neuron] += learningRate * delta;
                });
            }

            return totalError;
        }

        private double Sigmoid(double x) => 1.0 / (1.0 + Math.Exp(-x));

        private double SigmoidDerivative(double x) => x * (1.0 - x);
    }
}
