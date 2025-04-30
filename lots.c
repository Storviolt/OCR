#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <math.h>
#include <dirent.h>
#include <string.h>
#include <time.h>

#define IMAGE_SIZE 28 * 28  // Taille de l'image 28x28
#define NB_CLASSES 26
#define HIDEN_SIZE2 128
#define ENTRY_SIZE 784
#define NB_EPOCHS 10
#define VFD (ENTRY_SIZE * HIDEN_SIZE2 + HIDEN_SIZE2 * NB_CLASSES)
#define AZY HIDEN_SIZE2
#define poert NB_CLASSES

void save_weight(float* weight, int tai, const char* path) {
    FILE* fich = fopen(path, "wb");
    if (fich != NULL) {
        fwrite(weight, sizeof(float), tai, fich);
        fclose(fich);
    } else {
        perror("Error when opening the file to save weights");
    }
}

float* charger_poids(int size, const char* chemin) {
    float* weight = malloc(size * sizeof(float));
    if (weight == NULL) 
    {
        perror("Memory allocation error for weights");
        return NULL;
    }
    FILE* fich = fopen(chemin, "rb");
    if (fich != NULL) {
        fread(weight, sizeof(float), size, fich);
        fclose(fich);
        return weight;
    } else {
        perror("Error when opening the file to load weights");
        free(weight);
        return NULL;
    }
}

void save_bias(float* biais, int size, const char* path) {
    FILE* fich = fopen(path, "wb");
    if (fich != NULL) {
        fwrite(biais, sizeof(float), size, fich);
        fclose(fich);
    } else {
        perror("Error when opening file to save bias");
    }
}


float* charger_biais(int size, const char* path) {
    float* biais = malloc(size * sizeof(float));
    if (biais == NULL) {
        perror("Memory allocation error for biases");
        return NULL;
    }
    FILE* fich = fopen(path, "rb");
    if (fich != NULL) {
        fread(biais, sizeof(float), size, fich);
        fclose(fich);
        return biais;
    } else {
        perror("Error when opening the file to load biases");
        free(biais);
        return NULL;
    }
}

float* init_weight() {
    float* weight = malloc(VFD * sizeof(float));
    if (weight == NULL) {
        perror("Memory allocation error for weights");
        return NULL;
    }
    srand(time(NULL));
    for (int i = 0; i < VFD; i++) {
        weight[i] = ((float)rand() / RAND_MAX) * 0.2f - 0.01f;
    }
    return weight;
}

float* init_bias(int size) {
    float* biais = malloc(size * sizeof(float));
    if (biais == NULL) {
        perror("Memory allocation error for biases");
        return NULL;
    }
    for (int i = 0; i < size; i++) {
        biais[i] = 0.0f;
    }
    return biais;
}

void charger_images_entrainement(const char* dossier, float images[][IMAGE_SIZE], int* labels, int* nb_images) {
    DIR* dir;
    struct dirent* ent;
    if ((dir = opendir(dossier)) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_type == DT_REG) {
                char chemin_image[257];
                snprintf(chemin_image, sizeof(chemin_image), "%s/%s", dossier, ent->d_name);

                SDL_Surface* image = IMG_Load(chemin_image);
                if (image) {
                    if (image->w == 28 && image->h == 28) {
                        Uint32* pixels = (Uint32*)image->pixels;
                        for (int i = 0; i < IMAGE_SIZE; i++) {
                            Uint8 r, g, b;
                            SDL_GetRGB(pixels[i], image->format, &r, &g, &b);
                            float pixel_value = (r + g + b) / 3.0f;
                            images[*nb_images][i] = pixel_value / 255.0f;
                        }
                        FILE* label_file = fopen("labels.txt", "r");
                        if (label_file) {
                            fscanf(label_file, "%d", &labels[*nb_images]);
                            fclose(label_file);
                        } else {
                            fprintf(stderr, "Error when opening the label file.\n");
                        }
                        (*nb_images)++;
                    } else {
                        fprintf(stderr, "Wrong dimensions for image : %s\n", chemin_image);
                    }
                    SDL_FreeSurface(image);
                } else {
                    fprintf(stderr, "Error when opening image file : %s\n", chemin_image);
                }
            }
        }
        closedir(dir);
    } else {
        perror("Error when opening drive folder");
    }
}

void charger_images_test(const char* dossier, float images[][IMAGE_SIZE], int* nb_images) {
    DIR* dir;
    struct dirent* ent;
    if ((dir = opendir(dossier)) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_type == DT_REG) {
                char chemin_image[257];
                snprintf(chemin_image, sizeof(chemin_image), "%s/%s", dossier, ent->d_name);
                SDL_Surface* image = IMG_Load(chemin_image);
                if (image) {
                    if (image->w == 28 && image->h == 28) {
                        
                        Uint32* pixels = (Uint32*)image->pixels;
                        for (int i = 0; i < IMAGE_SIZE; i++) {
                            Uint8 r, g, b ;
                            SDL_GetRGB(pixels[i], image->format, &r, &g, &b);

                            
                            float pixel_value = (r + g + b) / 3.0f;
                            images[*nb_images][i] = pixel_value / 255.0f;
                        }
                        (*nb_images)++;
                    } else {
                        fprintf(stderr, "Wrong dimensions for image : %s\n", chemin_image);
                    }
                    SDL_FreeSurface(image);
                } else {
                    fprintf(stderr, "Error when opening image file : %s\n", chemin_image);
                }
            }
        }
        closedir(dir);
    } else {
        perror("Error when opening the test folder");
    }
}

int argmax(float* exit, int size) {
    int max_index = 0;
    for (int i = 1; i < size; i++) {
        if (exit[i] > exit[max_index]) {
            max_index = i;
        }
    }
    return max_index;
}

float relu(float x) { return (x > 0) ? x : 0;}

void softmax(float* sortie, int taille) {
    float somme = 0.0f;
    float max_val = sortie[0];
    for (int i = 1; i < taille; i++) {
        if (sortie[i] > max_val) {
            max_val = sortie[i];
        }
    }
    for (int i = 0; i < taille; i++) {
        sortie[i] = exp(sortie[i] - max_val);
        somme += sortie[i];
    }
    for (int i = 0; i < taille; i++) {
        sortie[i] /= somme;  
    }
}

float calculer_perte(float* exit, int label) {
    return -log(exit[label]);
}

void retropropagation(float* image, float* poids, float* biais_cachee, float* biais_sortie, float* sortie, int label, float taux_apprentissage) {
    float d_sortie[NB_CLASSES];
    for (int k = 0; k < NB_CLASSES; k++) {
        d_sortie[k] = sortie[k];
        if (k == label) {
            d_sortie[k] -= 1;
        }
    }

    
    float d_couche_cachee[HIDEN_SIZE2] = {0};
    for (int k = 0; k < NB_CLASSES; k++) {
        for (int j = 0; j < HIDEN_SIZE2; j++) {
            d_couche_cachee[j] += d_sortie[k] * poids[ENTRY_SIZE * HIDEN_SIZE2 + j * NB_CLASSES + k];
        }
    }

    
    for (int j = 0; j < HIDEN_SIZE2; j++) {
        if (d_couche_cachee[j] > 0) {
            d_couche_cachee[j] *= 1;
        } else {
            d_couche_cachee[j] *= 0;
        }
        
        biais_cachee[j] -= taux_apprentissage * d_couche_cachee[j];
    }

    
    for (int k = 0; k < NB_CLASSES; k++) {
        for (int j = 0; j < HIDEN_SIZE2; j++) {
            poids[ENTRY_SIZE * HIDEN_SIZE2 + j * NB_CLASSES + k] -= taux_apprentissage * d_sortie[k] * d_couche_cachee[j];
        }
        
        biais_sortie[k] -= taux_apprentissage * d_sortie[k];
    }

    
    for (int j = 0; j < HIDEN_SIZE2; j++) {
        for (int i = 0; i < ENTRY_SIZE; i++) {
            poids[i * HIDEN_SIZE2 + j] -= taux_apprentissage * d_couche_cachee[j] * image[i];
        }
    }
}

void propagation_avant(float* image, float* poids, float* biais_cachee, float* biais_sortie, float* sortie, int taille_image, int taille_poids) {
    float couche_cachee[HIDEN_SIZE2];
    
    if (image == NULL || poids == NULL || sortie == NULL || biais_cachee == NULL || biais_sortie == NULL) {
        printf("One of the pointers is NULL\n");
        return;
    }
    
    if (taille_image != ENTRY_SIZE) {
        printf("Error: image size is not correct. Expected : %d, Received : %d\n", ENTRY_SIZE, taille_image);
        return;
    }
    if (taille_poids < (ENTRY_SIZE * HIDEN_SIZE2 + HIDEN_SIZE2 * NB_CLASSES)) {
        printf("Erreur : la taille des poids n'est pas suffisante.") ;
        return;
    }

    
    for (int j = 0; j < HIDEN_SIZE2; j++) {
        couche_cachee[j] = biais_cachee[j];  
        for (int i = 0; i < ENTRY_SIZE; i++) {
            couche_cachee[j] += image[i] * poids[i * HIDEN_SIZE2 + j]; 
        }
        couche_cachee[j] = relu(couche_cachee[j]);
    }

    
    for (int k = 0; k < NB_CLASSES; k++) {
        sortie[k] = biais_sortie[k]; 
        for (int j = 0; j < HIDEN_SIZE2; j++) {
            sortie[k] += couche_cachee[j] * poids[ENTRY_SIZE * HIDEN_SIZE2 + j * NB_CLASSES + k];  
        }
    }
    softmax(sortie, NB_CLASSES);
}

void entrainement() {
    float taux_apprentissage = 0.1;
    const char* chemin_poids = "weight/weight.bin";
    const char* chemin_biais_cachee = "weight/biais_hidden.bin";
    const char* chemin_biais_sortie = "weight/biais_exit.bin";
    float* poids = charger_poids(VFD, chemin_poids);
    float* biais_cachee = charger_biais(AZY, chemin_biais_cachee);
    float* biais_sortie = charger_biais(poert, chemin_biais_sortie);
    if (poids == NULL) {
        poids = init_weight(); 
    }
    if (biais_cachee == NULL) {
        biais_cachee = init_bias(AZY);  
    }
    if (biais_sortie == NULL) {
        biais_sortie = init_bias(poert);  
    }
    float images[2600][IMAGE_SIZE];  
    int labels[2600];                  
    int nb_images = 0;
    charger_images_entrainement("images_entrainement", images, labels, &nb_images);
    printf("number_images: %d\n", nb_images);
    if (nb_images <= 0) {
        printf("Error image.");
        return;
    }
    for (int epoch = 0; epoch < NB_EPOCHS; epoch++) {
        float perte_totale = 0.0f;  
        for (int i = 0; i < nb_images; i++) {
            float sortie[NB_CLASSES];
            propagation_avant(images[i], poids, biais_cachee, biais_sortie, sortie, ENTRY_SIZE, VFD);
            float perte = calculer_perte(sortie, labels[i]);
            perte_totale += perte; 
            retropropagation(images[i], poids, biais_cachee, biais_sortie, sortie, labels[i], taux_apprentissage);
        }
        printf("Epoch %d, average lose: %f\n", epoch, perte_totale / nb_images);
    }
    save_weight(poids, VFD, chemin_poids);
    save_bias(biais_cachee, AZY, chemin_biais_cachee);
    save_bias(biais_sortie, poert, chemin_biais_sortie);
    free(poids);
    free(biais_cachee);
    free(biais_sortie);
}

void test() {
    const char* chemin_poids = "weight/weight.bin";
    const char* chemin_biais_cachee = "weight/biais_hidden.bin";
    const char* chemin_biais_sortie = "weight/biais_exit.bin";
    float* poids = charger_poids(VFD, chemin_poids);
    float* biais_cachee = charger_biais(AZY, chemin_biais_cachee);
    float* biais_sortie = charger_biais(poert, chemin_biais_sortie);
    if (poids == NULL || biais_cachee == NULL || biais_sortie == NULL) {
        fprintf(stderr, "Aucun poids ou biais chargé, veuillez entraîner le modèle d'abord.\n");
        return;
    }
    float images[100][IMAGE_SIZE]; 
    int nb_images = 0;
    charger_images_test("images_test", images, &nb_images);
    for (int i = 0; i < nb_images; i++) {
        float sortie[NB_CLASSES];  
        int taille_image = ENTRY_SIZE; 
        int taille_poids = ENTRY_SIZE * HIDEN_SIZE2 + HIDEN_SIZE2 * NB_CLASSES; 
        propagation_avant(images[i], poids, biais_cachee, biais_sortie, sortie, taille_image, taille_poids);
        int prediction = argmax(sortie, NB_CLASSES);
        printf("Image %d : Prédiction = %c\n", i, 'A' + prediction);  
    }
    free(poids);
    free(biais_cachee);
    free(biais_sortie);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <mode>\n", argv[0]);
        return 1;
    }
    if (strcmp(argv[1], "entrainement") == 0) {
        entrainement();
    } else if (strcmp(argv[1], "test") == 0) {
        test();
    } else {
        fprintf(stderr, "Use 'entrainement' ou 'test'.\n");
        return 1;
    }
    return 0;
}