﻿using System;
using System.Diagnostics;
using System.Threading.Tasks;

namespace NeuralNetwork1
{
    public class EnhancedNetwork : BaseNetwork
    {
        private int[] structure;
        private double[][] neurons;
        private double[][][] weights;
        private double[][] biases;
        private double[][][] weightVelocities;
        private double[][] biasVelocities;

        private double learningRate = 0.1;
        private double momentum = 0.9;
        private double regularization = 0.0005;

        private Random random;
        public Stopwatch stopWatch = new Stopwatch();

        public EnhancedNetwork(int[] structure)
        {
            this.structure = structure;
            neurons = new double[structure.Length][];
            weights = new double[structure.Length - 1][][];
            biases = new double[structure.Length - 1][];
            weightVelocities = new double[structure.Length - 1][][];
            biasVelocities = new double[structure.Length - 1][];

            random = new Random();

            for (int i = 0; i < structure.Length; i++)
                neurons[i] = new double[structure[i]];

            for (int i = 0; i < structure.Length - 1; i++)
            {
                int currentLayerSize = structure[i];
                int nextLayerSize = structure[i + 1];

                weights[i] = new double[nextLayerSize][];
                weightVelocities[i] = new double[nextLayerSize][];
                biases[i] = new double[nextLayerSize];
                biasVelocities[i] = new double[nextLayerSize];

                for (int j = 0; j < nextLayerSize; j++)
                {
                    weights[i][j] = new double[currentLayerSize];
                    weightVelocities[i][j] = new double[currentLayerSize];
                    for (int k = 0; k < currentLayerSize; k++)
                        weights[i][j][k] = random.NextDouble() * 2 - 1;

                    biases[i][j] = random.NextDouble() * 2 - 1;
                }
            }
        }

        public override int Train(Sample sample, double acceptableError, bool parallel)
        {
            double error;
            int iterations = 0;

            do
            {
                error = TrainSample(sample.input, sample.Output);
                iterations++;
            } while (error > acceptableError);

            return iterations;
        }

        public override double TrainOnDataSet(SamplesSet samplesSet, int epochsCount, double acceptableError, bool parallel)
        {
            double totalError = double.PositiveInfinity;

            stopWatch.Restart();

            for (int epoch = 0; epoch < epochsCount; epoch++)
            {
                totalError = 0;

                if (parallel)
                {
                    Parallel.ForEach(samplesSet.samples, sample =>
                    {
                        totalError += TrainSample(sample.input, sample.Output);
                    });
                }
                else
                {
                    foreach (Sample sample in samplesSet.samples)
                        totalError += TrainSample(sample.input, sample.Output);
                }

                totalError /= samplesSet.Count;

                if (totalError <= acceptableError)
                    break;

                OnTrainProgress((double)epoch / epochsCount, totalError, stopWatch.Elapsed);
            }

            OnTrainProgress(1.0, totalError, stopWatch.Elapsed);
            stopWatch.Stop();

            return totalError;
        }

        protected override double[] Compute(double[] input)
        {
            Array.Copy(input, neurons[0], input.Length);

            for (int layer = 0; layer < weights.Length; layer++)
            {
                Parallel.For(0, neurons[layer + 1].Length, neuron =>
                {
                    double sum = biases[layer][neuron];

                    for (int prevNeuron = 0; prevNeuron < neurons[layer].Length; prevNeuron++)
                        sum += neurons[layer][prevNeuron] * weights[layer][neuron][prevNeuron];

                    neurons[layer + 1][neuron] = ReLU(sum);
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
                    double delta = errors[layer + 1][neuron] * ReLUDerivative(neurons[layer + 1][neuron]);

                    for (int prevNeuron = 0; prevNeuron < neurons[layer].Length; prevNeuron++)
                    {
                        double gradient = delta * neurons[layer][prevNeuron];
                        weightVelocities[layer][neuron][prevNeuron] = momentum * weightVelocities[layer][neuron][prevNeuron] + learningRate * (gradient - regularization * weights[layer][neuron][prevNeuron]);
                        weights[layer][neuron][prevNeuron] += weightVelocities[layer][neuron][prevNeuron];

                        errors[layer][prevNeuron] += delta * weights[layer][neuron][prevNeuron];
                    }

                    biasVelocities[layer][neuron] = momentum * biasVelocities[layer][neuron] + learningRate * delta;
                    biases[layer][neuron] += biasVelocities[layer][neuron];
                });
            }

            return totalError;
        }

        private double ReLU(double x) => Math.Max(0, x);

        private double ReLUDerivative(double x) => x > 0 ? 1 : 0;
    }
}
