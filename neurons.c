#include <stdlib.h>
#include <math.h>
#include <stdio.h>

double generation() // generate a random number between -0.5 and 0.5
{
    return ((double)rand() / RAND_MAX) - 0.5;
}

double activation_function(double n) // sigmoid activation function
{
    double res = 1/ (1+exp(-n));
    return res;
}

const int num_inputs = 2;
const int num_hidden = 2;

double hidden_weights[2][2];
double hidden_biases[2];

double output_weights[2];
double output_bias;

double learning_rate = 0.8;

void initialize_weights_and_biases()
{
    hidden_weights[0][0] = generation();
    hidden_weights[0][1] = generation();
    hidden_weights[1][0] = generation();
    hidden_weights[1][1] = generation();
    

    hidden_biases[0] = generation();
    hidden_biases[1] = generation();

    output_weights[0] = generation();
    output_weights[1] = generation();

    output_bias = generation();

}

double tempo[2]; //store the value of hidden neurons
double forward_propagation(double begin[])
{
    double x = begin[0]*hidden_weights[0][0] + 
    begin[1]*hidden_weights[0][1] + hidden_biases[0];
    double y = begin[0]*hidden_weights[1][0] + 
    begin[1]*hidden_weights[1][1] + hidden_biases[1];

    tempo[0] = activation_function(x);
    tempo[1] = activation_function(y);

    double res = tempo[0] * output_weights[0] + 
    tempo[1] * output_weights[1] + output_bias;
    return activation_function(res);
}

double backpropagation(double begin[], double target)
{
    double output = forward_propagation(begin);
    double loss = -(output * log(output) + (1 - output) * log(1 - output));

    double output_gradient = output - target;
    
    double output_weight_gradients[2];
    output_weight_gradients[0] = output_gradient *
    output * (1 - output) * tempo[0];
    output_weight_gradients[1] = output_gradient *
    output * (1 - output) * tempo[1];
    output_weights[0] -= learning_rate *
    output_weight_gradients[0];
    output_weights[1] -= learning_rate *
    output_weight_gradients[1];


    double output_bias_gradient = output_gradient *
    output * (1 - output);
    output_bias -= learning_rate * output_bias_gradient;


    double hidden_gradients[2];
    for (int i = 0; i < 2; i++) {
        hidden_gradients[i] = output_gradient *
        output * (1 - output) * output_weights[i];
    }

    
    double hidden_weight_gradients[2][2];
    for (int i = 0; i < 2; i++) {
        double input_gradient = hidden_gradients[i] *
        tempo[i] * (1 - tempo[i]);
        for (int j = 0; j < 2; j++) {
            hidden_weight_gradients[j][i] = input_gradient * begin[j];
            hidden_weights[j][i] -= learning_rate *
            hidden_weight_gradients[j][i];
        }
        hidden_biases[i] -= learning_rate * input_gradient;
    }
    return loss;
}

int main()
{
    initialize_weights_and_biases();
    double inputs[4][2] = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
    double targets[4] = {1, 0, 0, 1};

    for (int epoch = 0; epoch < 10000000; epoch++) 
    {
        for (int i = 0; i < 4; i++) 
        {
            backpropagation(inputs[i], targets[i]);
        }
    }
    for (int i = 0; i < 4; i++) 
    {
        double output = forward_propagation(inputs[i]);
        printf("Input: (%.0f, %.0f) Output: %.6f\n",\
         inputs[i][0], inputs[i][1], output);
    }
    
    return 0;
}
