#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void escala_g(FILE *imagen_init, FILE *imagen_gris, unsigned char encabezado[], int tam, int a_total_imagen, int **mat_gris);
void llenar_mat(int *mat_xy, int *mat, int *size);
void get_submat(int *mat_img, int i, int j, int *mat_temp, int ancho);
int sum_mat(int *mat_temp);
void mat_conv(int operador[3], int *mat_temp);
void blan_neg(int *mat_xy, int *size);
void crear_img(FILE *imagen_filtro, int **mat_xy, int ancho, int alto);
void filtro_x(int *mat_x, int ancho, int alto);
void filtro_y(int *mat_y, int ancho, int alto);
void filtro_fin(int *mat_x, int *mat_y, int *size);

int main(){
///Lectura del encabezado imagen inicial
unsigned char encabezado[54];
FILE *imagen_init = fopen("lena_color.bmp", "rb");
fread(encabezado, sizeof(unsigned char), 54, imagen_init);
int ancho = *(int*)&encabezado[18], alto = *(int*)&encabezado[22];
int a_total_imagen = alto*ancho;

///Funcion que escala a gris
FILE *imagen_gris = fopen("lena_g.bmp", "wb+");
int *mat_gris = (int*)malloc(sizeof(int)*a_total_imagen);
escala_g(imagen_init, imagen_gris, encabezado, 54, a_total_imagen, &mat_gris);

/////Abrir la imagen en X
FILE *imagen_fx = fopen("lena_x.bmp", "wb+");
fread(encabezado, sizeof(unsigned char), 54, imagen_gris);
fwrite(encabezado, sizeof(unsigned char), 54, imagen_fx);

///Se crea la matriz y se aplica el filtro de sobel
int *mat_fx = (int*)malloc(sizeof(int)*a_total_imagen);
llenar_mat(mat_fx, mat_gris, &a_total_imagen);
filtro_x(mat_fx, ancho, alto);
blan_neg(mat_fx, &a_total_imagen);
crear_img(imagen_fx, &mat_fx, ancho, alto);

///Abrir la imagen en Y
FILE *imagen_fy = fopen("lena_y.bmp", "wb+");
fwrite(encabezado, sizeof(unsigned char), 54, imagen_fy);

///Se crea mat y se aplica el filtro de sobel
int *mat_fy = (int*)malloc(sizeof(int)*a_total_imagen);
llenar_mat(mat_fy, mat_gris, &a_total_imagen);
filtro_y(mat_fy, ancho, alto);
blan_neg(mat_fy, &a_total_imagen);
crear_img(imagen_fy, &mat_fy, ancho, alto);

///Se juntan X y Y
FILE *imagen_fin = fopen("lain_final.bmp", "wb");
fseek(imagen_gris, 0, SEEK_SET);
fwrite(encabezado, sizeof(unsigned char), 54, imagen_fin);
filtro_fin(mat_fx, mat_fy, &a_total_imagen);
crear_img(imagen_fin, &mat_fx, ancho, alto);

fclose(imagen_init);
fclose(imagen_gris);
fclose(imagen_fx);
fclose(imagen_fy);
fclose(imagen_fin);
return 0;
}


void escala_g(FILE *imagen_init, FILE *imagen_gris, unsigned char encabezado[], int tam, int a_total_imagen, int **mat_gris){
    fwrite(encabezado, sizeof(unsigned char), 54, imagen_gris);
    int i, y;
    unsigned char cambio[a_total_imagen][2];    
    for(i=0; i<a_total_imagen; i++){
        cambio[i][2] = getc(imagen_init);
        cambio[i][1] = getc(imagen_init);
        cambio[i][0] = getc(imagen_init);
    
        *(*mat_gris+i) = (cambio[i][0]*0.3) + (cambio[i][1]*0.59) + (cambio[i][2]*0.11); 

        putc(*(*mat_gris+i),imagen_gris);
        putc(*(*mat_gris+i),imagen_gris);
        putc(*(*mat_gris+i),imagen_gris);
    }
}

void llenar_mat(int *mat_xy, int *mat_gris, int *size){
    for (int i = 0; i < *size; i++)
        mat_xy[i] = mat_gris[i];
}   

void get_submat(int *mat_img, int i, int j, int *mat_temp, int ancho){
    for (int k = 0; k < 3; k++)
        for (int l = 0; l < 3; l++)
            mat_temp[k*3+l] = mat_img[(k+i)*ancho+(j+l)];
}

int sum_mat(int *mat_temp){ 
    int suma = 0;
    for (int i = 0; i < 9; i++)
        suma += mat_temp[i];
    return suma;
}

void mat_conv(int operador[9], int *mat_temp){
    for (int i=0; i<9; i++)
        mat_temp[i] *= operador[i];
}

void blan_neg(int *mat_xy, int *size){
    for (int i = 0; i < *size; i++)
            mat_xy[i] = (mat_xy[i] > 125) ? 255 : 0;
}

void crear_img(FILE *imagen_filtro, int **mat_xy, int ancho, int alto){
    for (int i = 0; i < ancho*alto; i++){
            //putc(*(*mat_xy+i), imagen_filtro);
            putc(*(*mat_xy+i), imagen_filtro);
            putc(*(*mat_xy+i), imagen_filtro);
            putc(*(*mat_xy+i), imagen_filtro);
        }
}

void filtro_x(int *mat_x, int ancho, int alto){
    int operador_x[9] = {-1, 0, 1,-2, 0, 2,-1, 0, 1};
    int *mat_t = (int*)malloc(9*sizeof(int));

    for (int i = 0; i < alto-2; i++)
        for (int j = 0; j < ancho-2; j++){
            get_submat(mat_x, i, j, mat_t, ancho);
            mat_conv(operador_x, mat_t);
            mat_x[i*ancho+j] = sum_mat(mat_t);
        }    
    free(mat_t);
}   

void filtro_y(int *mat_y, int ancho, int alto){
    int operador_y[9] = {-1, -2, -1, 0, 0, 0, 1, 2, 1};
    int *mat_t = (int*)malloc(sizeof(int)*9);

    for (int i = 1; i < alto-2; i++)
        for (int j = 1; j < ancho-2; j++){
            get_submat(mat_y, i, j, mat_t, ancho);
            mat_conv(operador_y, mat_t);
            mat_y[i*ancho+j] = sum_mat(mat_t);
        }
    free(mat_t);
}   

void filtro_fin(int *mat_x, int *mat_y, int *size){
    for (int i = 0; i < *size; i++)
        mat_x[i] = (mat_x[i]) + (mat_y[i]);
}