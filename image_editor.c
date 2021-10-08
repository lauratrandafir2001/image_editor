#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include<string.h>

int round_no(double x)
{
	if ((x - (x / 10)) * 1000 > 500)
		return (double)(x + 0.5);
	else
		return (double)(x - 0.5);
}

typedef struct{
	char type[2];
	void *imagine_arr;
	int n, m;
} imagine;

int min(int a, int b)
{
	if (a < b)
		return a;
	return b;
}

void memfree(imagine img)
{
	if (strcmp(img.type, "P1") == 0 || strcmp(img.type, "P2") == 0 ||
		strcmp(img.type, "P4") == 0 || strcmp(img.type, "P5") == 0) {
		for (int i = 0; i < img.n; i++)
			free(((int **)(img.imagine_arr))[i]);
	} else {
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < img.n; j++)
				free(((int ***)(img.imagine_arr))[i][j]);
		}
		for (int i = 0; i < 3; i++)
			free(((int ***)(img.imagine_arr))[i]);
	}
	free(img.imagine_arr);
}

int **alloc_matrix(int n, int m, FILE *fisier, int binary)
{										//functie pentru matrici alb-negru
	int **a = (int **)calloc(n, sizeof(int *));
	unsigned char temp;
	if (!a) {
		printf("nu merge");
		return NULL;
	}
	for (int i = 0; i < n; i++) {
		a[i] = (int *)calloc(m, sizeof(int));
		if (!a[i])
			return NULL;
		for (int j = 0; j < m; j++)
			if (binary == 0) {			//verific daca fisierul e binar
				fscanf(fisier, "%d", &a[i][j]);
			} else {
				fread(&temp, sizeof(temp), 1, fisier);
				a[i][j] = temp;
			}
	}
	return a;
}

int ***alloc_matrix3d(int n, int m, FILE *fisier, int binary)
{					//functie pentru matrici color
	unsigned char temp;
	int ***a = (int ***)calloc(3, sizeof(int **));
	if (!a) {
		printf("nu merge");
		return NULL;
	}
	for (int i = 0; i < 3; i++) {
		a[i] = (int **)calloc(n, sizeof(int *));
		for (int j = 0; j < n; j++) {
			a[i][j] = (int *)calloc(m, sizeof(int));
			if (!a[i][j])
				return NULL;
		}
	}
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++)
			if (binary == 0) {
				fscanf(fisier, "%d ", &a[0][i][j]);
				fscanf(fisier, "%d ", &a[1][i][j]);
				fscanf(fisier, "%d ", &a[2][i][j]);
			} else {
				fread(&temp, 1, 1, fisier);
				a[0][i][j] = temp;
				fread(&temp, 1, 1, fisier);
				a[1][i][j] = temp;
				fread(&temp, 1, 1, fisier);
				a[2][i][j] = temp;
			}
	}
	return a;
}

imagine read(char *num, int *cnt)
{
	int max_val;
	imagine img;
	img.imagine_arr = NULL;
	FILE *fisier = fopen(num, "rbw");
		if (!fisier) {//verificam daca exista fisierul
			printf("Failed to load %s\n", num);
			*cnt = 0;
			return img;
		}
		if (fisier) {
			fscanf(fisier, "%s", img.type);
			fscanf(fisier, "%d %d\n", &img.m, &img.n);
		if (strcmp(img.type, "P2") == 0 || strcmp(img.type, "P3") == 0 ||
			strcmp(img.type, "P5") == 0 || strcmp(img.type, "P6") == 0) {
			fscanf(fisier, "%d ", &max_val);
		}
		if (strcmp(img.type, "P1") == 0 || strcmp(img.type, "P2") == 0)
			img.imagine_arr = alloc_matrix(img.n, img.m, fisier, 0);
		else if (strcmp(img.type, "P3") == 0)
			img.imagine_arr = alloc_matrix3d(img.n, img.m, fisier, 0);
		else if (strcmp(img.type, "P5") == 0 || strcmp(img.type, "P4") == 0)
			img.imagine_arr = alloc_matrix(img.n, img.m, fisier, 1);
		else if (strcmp(img.type, "P6") == 0)
			img.imagine_arr = alloc_matrix3d(img.n, img.m, fisier, 1);
		printf("Loaded %s\n", num);
		*cnt = *cnt + 1;
		fclose(fisier);
		return img;
	}
	return img;
}

imagine sepia_function(imagine img, int x1, int y1, int x2, int y2)
{
	double new_rred, new_ggreeen, new_bblue;
	if (!(strcmp(img.type, "P3") == 0 || strcmp(img.type, "P6") == 0)) {
		printf("Sepia filter not available\n");
		return img;
	}
	imagine new_img; //copie a struct
	strcpy(new_img.type, img.type);
	new_img.n = img.n;
	new_img.m = img.m;
	int ***imagine_arr = (int ***)calloc(3, sizeof(int **));
	int ***img_arr_old = (int ***)img.imagine_arr;
	if (!imagine_arr) {
		printf("nu merge");
		return img;
	}
	for (int i = 0; i < 3; i++) {
		imagine_arr[i] = (int **)calloc(new_img.n, sizeof(int *));
		for (int j = 0; j < new_img.n; j++) {
			imagine_arr[i][j] = (int *)calloc(new_img.m, sizeof(int));
			if (!imagine_arr[i][j])
				return img;
		}
	}		//in urmatoarea secv calc fiecare val pt zona selectata
	for (int i = 0; i < new_img.n; i++) {//si aplic round pt rotunjiri
		for (int j = 0; j < new_img.m; j++) {//apoi fac cast la int
			if (y1 <= i && y2 >= i && x1 <= j && x2 >= j) {
				new_rred = (0.393 * img_arr_old[0][i][j]) +
									(0.769 * img_arr_old[1][i][j]) +
									(0.189 * img_arr_old[2][i][j]);
				imagine_arr[0][i][j] = min(round_no(new_rred), 255);
				new_ggreeen = (0.349 * img_arr_old[0][i][j]) +
										(0.686 * img_arr_old[1][i][j]) +
										(0.168 * img_arr_old[2][i][j]);
				imagine_arr[1][i][j] = min(round_no(new_ggreeen), 255);
				new_bblue = (0.272 * img_arr_old[0][i][j]) +
						(0.534 * img_arr_old[1][i][j]) +
						(0.131 * img_arr_old[2][i][j]);
				imagine_arr[2][i][j] = min(round_no(new_bblue), 255);
				} //restul matricii se copiaza(mai jos)
			if (!(y1 <= i && y2 >= i && x1 <= j && x2 >= j)) {
				imagine_arr[0][i][j] = img_arr_old[0][i][j];
				imagine_arr[1][i][j] = img_arr_old[1][i][j];
				imagine_arr[2][i][j] = img_arr_old[2][i][j];
			}
		}
	}
	new_img.imagine_arr = imagine_arr;
	memfree(img);//eliberez vechea struct
	printf("Sepia filter applied\n");
	return new_img;
}

imagine grayscale_function(imagine img, int x1, int y1, int x2, int y2)
{
	if (!(strcmp(img.type, "P3") == 0 || strcmp(img.type, "P6") == 0)) {
		printf("Grayscale filter not available\n");
		return img;
	}
	imagine new2_img;//copie
	strcpy(new2_img.type, img.type);
	new2_img.n = img.n;
	new2_img.m = img.m;
	double new_red, new_green, new_blue;
	int ***imagine_arr = (int ***)calloc(3, sizeof(int **));
	int ***img_arr_old2 = (int ***)img.imagine_arr;
	if (!imagine_arr) {
		printf("nu merge");
		return img;
	}
	for (int i = 0; i < 3; i++) {
		imagine_arr[i] = (int **)calloc(new2_img.n, sizeof(int *));
		for (int j = 0; j < new2_img.n; j++) {
			imagine_arr[i][j] = (int *)calloc(new2_img.m, sizeof(int));
			if (!imagine_arr[i][j])
				return img;
		}
	}
	for (int i = 0; i < new2_img.n; i++) {
		for (int j = 0; j < new2_img.m; j++) {
			if (y1 <= i && y2 >= i && x1 <= j && x2 >= j) {
				new_red = (img_arr_old2[0][i][j] +
					img_arr_old2[1][i][j] + img_arr_old2[2][i][j]) / 3;
				imagine_arr[0][i][j] = round_no(new_red);
				new_green = (img_arr_old2[0][i][j] + img_arr_old2[1][i][j]
				+ img_arr_old2[2][i][j]) / 3;
				imagine_arr[1][i][j] = round_no(new_green);
				new_blue = (img_arr_old2[0][i][j] + img_arr_old2[1][i][j]
				+ img_arr_old2[2][i][j]) / 3;
				imagine_arr[2][i][j] = round_no(new_blue);
				}
			if (!(y1 <= i && y2 >= i && x1 <= j && x2 >= j)) {
				imagine_arr[0][i][j] = img_arr_old2[0][i][j];
				imagine_arr[1][i][j] = img_arr_old2[1][i][j];
				imagine_arr[2][i][j] = img_arr_old2[2][i][j];
			}
		}
	}
	new2_img.imagine_arr = imagine_arr;
	memfree(img);
	printf("Grayscale filter applied\n");
	return new2_img;
}

imagine crop_function(imagine img, int x1, int y1, int x2, int y2)
{
	imagine new4_img;
	strcpy(new4_img.type, img.type);
	new4_img.n = y2 - y1 + 1;
	new4_img.m = x2 - x1 + 1;
	if (strcmp(img.type, "P1") == 0 || strcmp(img.type, "P2") == 0 ||
		strcmp(img.type, "P4") == 0 || strcmp(img.type, "P5") == 0) {
		int **cropped_matrix = (int **)calloc(new4_img.n, sizeof(int *));
		int **imagine_arr_old4 = (int **)img.imagine_arr;
		if (!cropped_matrix) {
			printf("nu merge");
			exit(0);
		}
		for (int i = 0; i < new4_img.n; i++) {
			cropped_matrix[i] = (int *)calloc(new4_img.m, sizeof(int));
			if (!cropped_matrix[i])
				exit(0);
		}
		for (int i = y1; i <= y2; i++) {
			for (int j = x1; j <= x2; j++) {
				int temp = imagine_arr_old4[i][j];
				cropped_matrix[i - y1][j - x1] = temp;
			}
		}
		printf("Image cropped\n");
		new4_img.imagine_arr = cropped_matrix;
		memfree(img);//eliberez vechea struct
	}
	if (strcmp(img.type, "P3") == 0 || strcmp(img.type, "P6") == 0) {
		int ***cropped_matrix3d = (int ***)calloc(3, sizeof(int **));
		int ***img_arr_old5 = (int ***)img.imagine_arr;
		if (!cropped_matrix3d) {
			printf("nu merge");
			return img;
		}
		for (int i = 0; i < 3; i++) {
			cropped_matrix3d[i] = (int **)calloc(new4_img.n, sizeof(int *));
				for (int j = 0; j < new4_img.n; j++) {
					cropped_matrix3d[i][j] = (int *)calloc(new4_img.m,
						sizeof(int));
					if (!cropped_matrix3d[i][j])
						return img;
				}
		}
		for (int i = y1; i <= y2; i++) {
			for (int j = x1; j <= x2; j++)
				for (int k = 0 ; k < 3 ; k++) {
					int temp = img_arr_old5[k][i][j];
					cropped_matrix3d[k][i - y1][j - x1] = temp;
				}
		}
		printf("Image cropped\n");
		new4_img.imagine_arr = cropped_matrix3d;
		memfree(img);
	}
	return new4_img;
}

imagine rotate_all(imagine img)
{
	imagine new3_img;
	strcpy(new3_img.type, img.type);
	new3_img.n = img.m;
	new3_img.m = img.n;
	int **rotated_all_matrix;
	if (strcmp(img.type, "P1") == 0 || strcmp(img.type, "P2") == 0 ||
		strcmp(img.type, "P4") == 0 || strcmp(img.type, "P5") == 0) {
		rotated_all_matrix = (int **)calloc(new3_img.n, sizeof(int *));
		int **imagine_arr_old7 = (int **)img.imagine_arr;
			if (!rotated_all_matrix) {
				printf("nu merge");
				exit(0);
			}
	for (int i = 0; i < new3_img.n; i++) {
		rotated_all_matrix[i] = (int *)calloc(new3_img.m, sizeof(int));
		if (!rotated_all_matrix[i])
			exit(0);
	}
	for (int i = 0; i < new3_img.m; i++) {
		for (int j = 0; j < new3_img.n; j++)
			rotated_all_matrix[j][new3_img.m - 1 - i] = imagine_arr_old7[i][j];
	}
	new3_img.imagine_arr = rotated_all_matrix;//
	memfree(img);
}

	if (strcmp(img.type, "P3") == 0 || strcmp(img.type, "P6") == 0) {
		int ***rotated_all_3d_matrix = (int ***)calloc(3, sizeof(int **));
		int ***img_arr_old8 = (int ***)img.imagine_arr;
		if (!rotated_all_3d_matrix) {
			printf("nu merge");
			return img;
		}
	for (int i = 0; i < 3; i++) {
		rotated_all_3d_matrix[i] = (int **)calloc(new3_img.n, sizeof(int *));
		for (int j = 0; j < new3_img.n; j++) {
			rotated_all_3d_matrix[i][j] = (int *)calloc(new3_img.m,
			 sizeof(int));
			if (!rotated_all_3d_matrix[i][j])
				return img;
		}
	}
	for (int k = 0; k < 3; k++) {
		for (int i = 0; i < new3_img.m; i++) {
			for (int j = 0; j < new3_img.n; j++) {
				rotated_all_3d_matrix[k][j][new3_img.m - 1 - i] =
				img_arr_old8[k][i][j];
			}
		}
	}
	new3_img.imagine_arr = rotated_all_3d_matrix;
	memfree(img);
}

return new3_img;
}

imagine rotate_section(imagine img, int x1, int y1, int x2, int y2)
{
	imagine new3_img;
	strcpy(new3_img.type, img.type);
	new3_img.n = img.n;
	new3_img.m = img.m;
	if (strcmp(img.type, "P1") == 0 || strcmp(img.type, "P2") == 0 ||
		strcmp(img.type, "P4") == 0 || strcmp(img.type, "P5") == 0) {
		int **rotated_matrix = (int **)calloc(new3_img.n, sizeof(int *));
		int **imagine_arr_old3 = (int **)img.imagine_arr;
		if (!rotated_matrix) {
			printf("it's not working");
			exit(0);
		}
	for (int i = 0; i < new3_img.n; i++) {
		rotated_matrix[i] = (int *)calloc(new3_img.m, sizeof(int));
		if (!rotated_matrix[i])
			exit(0);
	}
	for (int i = 0 ; i < new3_img.n; i++) {
		for (int j = 0; j < new3_img.m; j++)
			rotated_matrix[i][j] = imagine_arr_old3[i][j];
	}
	for (int i = 0; i <= y2 - y1; i++) {
		for (int j = 0; j <= x2 - x1; j++)
			rotated_matrix[j + y1][x2 - i] = imagine_arr_old3[i + y1][j + x1];
	}
	new3_img.imagine_arr = rotated_matrix;
	memfree(img);
	}
	if (strcmp(img.type, "P3") == 0 || strcmp(img.type, "P6") == 0) {
		int ***rotated_3d_matrix = (int ***)calloc(3, sizeof(int **));
		int ***imagine_arr_old8 = (int ***)img.imagine_arr;
		if (!rotated_3d_matrix) {
			printf("it's not working");
			return img;
		}
		for (int i = 0; i < 3; i++) {
			rotated_3d_matrix[i] = (int **)calloc(new3_img.n, sizeof(int *));
			for (int j = 0; j < new3_img.n; j++) {
				rotated_3d_matrix[i][j] = (int *)calloc(new3_img.m,
				sizeof(int));
				if (!rotated_3d_matrix[i][j])
					return img;
			}
		}
		for (int i = 0; i < new3_img.n; i++) {
			for (int j = 0; j < new3_img.m; j++) {
				rotated_3d_matrix[0][i][j] = imagine_arr_old8[0][i][j];
				rotated_3d_matrix[1][i][j] = imagine_arr_old8[1][i][j];
				rotated_3d_matrix[2][i][j] = imagine_arr_old8[2][i][j];
			}
		}
		for (int i = 0; i <= y2 - y1; i++) {
			for (int j = 0; j <= x2 - x1; j++) {
				rotated_3d_matrix[0][j + y1][x2 - i] =
				imagine_arr_old8[0][i + y1][j + x1];
				rotated_3d_matrix[1][j + y1][x2 - i] =
				imagine_arr_old8[1][i + y1][j + x1];
				rotated_3d_matrix[2][j + y1][x2 - i] =
				imagine_arr_old8[2][i + y1][j + x1];
			}
		}
	new3_img.imagine_arr = rotated_3d_matrix;
	memfree(img);
	}

return new3_img;
}

imagine decides_what_type_of_rotation(imagine img, int angle, int *x1_adr,
									  int *y1_adr, int *x2_adr, int *y2_adr)
{
	int x1 = *x1_adr, x2 = *x2_adr, y1 = *y1_adr, y2 = *y2_adr;
	if (!(x1 == 0 && y1 == 0 && x2 == img.m - 1 && y2 == img.n - 1)) {
		if (x2 - x1 != y2 - y1 && !(x1 == 0 && y1 == 0 &&
									x2 == img.m && y2 == img.n)) {
			printf("The selection must be square\n");
			return img;
		}
		if (angle == 90 || angle == -270) {
			img = rotate_section(img, x1, y1, x2, y2);
			printf("Rotated %d\n", angle);
		}
		if (angle == 180 || angle == -180) {
			img = rotate_section(rotate_section(img, x1, y1, x2, y2),
								 x1, y1, x2, y2);
			printf("Rotated %d\n", angle);
		}
		if (angle == 270 || angle == -90) {
			img = rotate_section(rotate_section(rotate_section(img,
															   x1, y1, x2, y2),
								 x1, y1, x2, y2), x1, y1, x2, y2);
			printf("Rotated %d\n", angle);
		}
		if (angle == 360 || angle == 0 || angle == -360) {
			img = img;
			printf("Rotated %d\n", angle);
		}
	} //verific daca este selectata toata
	if (x1 == 0 && y1 == 0 && x2 == img.m - 1 && y2 == img.n - 1) {
		if (angle == 90 || angle == -270) {
			img = rotate_all(img);
			printf("Rotated %d\n", angle);
		}
		if (angle == 180 || angle == -180) {
			img = rotate_all(rotate_all(img));
			printf("Rotated %d\n", angle);
		}
		if (angle == 270 || angle == -90) {
			img = rotate_all(rotate_all(rotate_all(img)));
			printf("Rotated %d\n", angle);
		}
		if (angle == 360 || angle == 0 || angle == -360) {
			img = img;
			printf("Rotated %d\n", angle);
		}
		*x2_adr = img.m - 1;
		*y2_adr = img.n - 1;
	}
	if (angle != 90 && angle != -90 && angle != 180 &&
		angle != -180 && angle != 270 && angle != -270 &&
		angle != 360 && angle != 0 && angle != -360)
		printf("Unsupported rotation angle\n");
return img;
}

void save_function(imagine img, char *num_out, char *ascii_var)
{
	unsigned  char temp;
	int **imag_arr = (int **)img.imagine_arr;
	FILE *file = fopen(num_out, "wtb");
	if (!file)
		exit(0);//(mai jos) pt salvarea ascii
	if (ascii_var && (strcmp(ascii_var, "ascii") == 0 ||
					  strcmp(ascii_var, "ascii\n") == 0)) {
		if (strcmp(img.type, "P4") == 0)
			fprintf(file, "P1\n");
		if (strcmp(img.type, "P5") == 0)
			fprintf(file, "P2\n");
		if (strcmp(img.type, "P6") == 0)
			fprintf(file, "P3\n");
		if (strcmp(img.type, "P1") == 0 ||
			strcmp(img.type, "P2") == 0 || strcmp(img.type, "P3") == 0)
			fprintf(file, "%s\n", img.type);
		fprintf(file, "%d %d\n", img.m, img.n);
		if (strcmp(img.type, "P2") == 0 ||
			strcmp(img.type, "P3") == 0 || strcmp(img.type, "P3") == 0 ||
			strcmp(img.type, "P5") == 0 || strcmp(img.type, "P6") == 0) {
			fprintf(file, "255\n");
		}
		if (strcmp(img.type, "P1") == 0 || strcmp(img.type, "P2") == 0 ||
			strcmp(img.type, "P4") == 0 || strcmp(img.type, "P5") == 0) {
			for (int i = 0; i < img.n; i++) {
				for (int j = 0; j < img.m; j++)
					fprintf(file, "%d ", imag_arr[i][j]);
				fprintf(file, "\n");
			}
		}
		if (strcmp(img.type, "P3") == 0 || strcmp(img.type, "P6") == 0) {
			int ***imag_arr = (int ***)img.imagine_arr;
			for (int i = 0; i < img.n; i++) {
				for (int j = 0; j < img.m; j++) {
					fprintf(file, "%d %d %d ", imag_arr[0][i][j],
							imag_arr[1][i][j], imag_arr[2][i][j]);
				}
				fprintf(file, "\n");
			}
		}
	}
	if (!ascii_var) {//pt salvarea in binar
		if (strcmp(img.type, "P1") == 0)
			fprintf(file, "P4\n");
		if (strcmp(img.type, "P2") == 0)
			fprintf(file, "P5\n");
		if (strcmp(img.type, "P3") == 0)
			fprintf(file, "P6\n");
		if (strcmp(img.type, "P4") == 0 || strcmp(img.type, "P5") == 0 ||
			strcmp(img.type, "P6") == 0)
			fprintf(file, "%s\n", img.type);
		fprintf(file, "%d %d\n", img.m, img.n);
		if (strcmp(img.type, "P4") == 0 || strcmp(img.type, "P5") == 0 ||
			strcmp(img.type, "P6") == 0 || strcmp(img.type, "P1") == 0 ||
				strcmp(img.type, "P2") == 0 || strcmp(img.type, "P3") == 0)
			fprintf(file, "255\n");
		if (strcmp(img.type, "P1") == 0 || strcmp(img.type, "P2") == 0 ||
			strcmp(img.type, "P4") == 0 || strcmp(img.type, "P5") == 0) {
			for (int i = 0; i < img.n; i++) {
				for (int j = 0; j < img.m; j++) {
					temp = imag_arr[i][j];
					fwrite(&temp, 1, 1, file);
				}
			}
		}
		if (strcmp(img.type, "P3") == 0 || strcmp(img.type, "P6") == 0) {
			int ***imag_arr = (int ***)img.imagine_arr;
			for (int i = 0; i < img.n; i++) {
				for (int j = 0; j < img.m; j++) {
					temp = imag_arr[0][i][j], fwrite(&temp, 1, 1, file);
					temp = imag_arr[1][i][j], fwrite(&temp, 1, 1, file);
					temp = imag_arr[2][i][j], fwrite(&temp, 1, 1, file);
				}
			}
		}
	}
	fclose(file);
}

int select_function(imagine img, int *x1_adr, int *y1_adr,
					int *x2_adr, int *y2_adr)
{
	int x1_temp, x2_temp, y1_temp, y2_temp;
	int ok = 1, x1 = *x1_adr, x2 = *x2_adr, y1 = *y1_adr, y2 = *y2_adr;
	char *x1_temp_char, *y1_temp_char, *x2_temp_char, *y2_temp_char, *no1;
	no1 = strtok(NULL, "\n");//pun in no1 ce este dupa SELECT
	if (no1[0] == 'A' && no1[1] == 'L' && no1[2] == 'L') {
		x1 = 0, x2 = img.m - 1, y1 = 0, y2 = img.n - 1;
		printf("Selected ALL\n");
	} //(mai jos)verific daca ce e dupa SELECT este cifra
	if ((no1[0] >= '0' && no1[0] <= '9') ||  no1[0] == '-') {
		x1_temp_char = strtok(no1, " ");
		y1_temp_char = strtok(NULL, " ");
		x2_temp_char = strtok(NULL, " ");
		y2_temp_char = strtok(NULL, " ");
		if (!(x1_temp_char && x2_temp_char && y1_temp_char && y2_temp_char)) {
			printf("Invalid command\n");//pt coordonate care nu exista
			return 0;
		}
		for (unsigned long k = 0; k < strlen(x1_temp_char); k++) {
			if (!((x1_temp_char[k] >= '0' && x1_temp_char[k] <= '9') ||
				  x1_temp_char[k] == '-')) {//verific in foruri daca fiecare
				printf("Invalid command\n");//coordonata este un numar
				ok = 0;
			}
			break;
		}
		for (unsigned long k = 0; k < strlen(y1_temp_char); k++) {
			if (!((y1_temp_char[k] >= '0' && y1_temp_char[k] <= '9') ||
				  y1_temp_char[k] == '-')) {
				printf("Invalid command\n");
				ok = 0;
			}
			break;
		}
		for (unsigned long k = 0; k < strlen(x2_temp_char); k++) {
			if (!((x2_temp_char[k] >= '0' && x2_temp_char[k] <= '9') ||
				  x2_temp_char[k] == '-')) {
				printf("Invalid command\n");
				ok = 0;
			}
			break;
		}
		for (unsigned long k = 0; k < strlen(y2_temp_char); k++) {
			if (!((y2_temp_char[k] >= '0' && y2_temp_char[k] <= '9') ||
				  y2_temp_char[k] == '-')) {
				printf("Invalid command\n");
				ok = 0;
			}
			break;
		}
		if (ok == 0)
			return 0;
		x1_temp = atoi(x1_temp_char), y1_temp = atoi(y1_temp_char);
		x2_temp = atoi(x2_temp_char), y2_temp = atoi(y2_temp_char);
		if (x1_temp < 0 || x2_temp < 0 || y1_temp < 0 || y2_temp < 0 ||
			x1_temp >= img.m || x2_temp > img.m || y1_temp >= img.n ||
		y2_temp > img.n || x1_temp == x2_temp || y1_temp == y2_temp) {
			printf("Invalid set of coordinates\n");
			return 0;
		}
	if (x1_temp < x2_temp)
		x1 = x1_temp, x2 = x2_temp - 1;
	else
		x1 = x2_temp, x2 = x1_temp - 1;
	if (y1_temp < y2_temp)
		y1 = y1_temp, y2 = y2_temp - 1;
	else
		y1 = y2_temp, y2 = y1_temp - 1;
	printf("Selected %d %d %d %d\n", x1, y1, x2 + 1, y2 + 1);
	}
	*x1_adr = x1, *x2_adr = x2, *y1_adr = y1, *y2_adr = y2;
	return 0;
}

int cnt_function(int *cnt) //verfica daca este vreo imagine
{							//incarcata
	if (*cnt == 0) {
		printf("No image loaded\n");
		return 0;
	}
	return 0;
}

void select_all(imagine img, int *x1_adr,
				int *y1_adr, int *x2_adr,
				int *y2_adr)
{
	int x1 = *x1_adr, x2 = *x2_adr, y1 = *y1_adr, y2 = *y2_adr;
	x1 = 0;
	x2 = img.m - 1;
	y1 = 0;
	y2 = img.n - 1;
	*x1_adr = x1;
	*x2_adr = x2;
	*y1_adr = y1;
	*y2_adr = y2;
}

void cnt_check(imagine img, int *cnt)
{
	if (*cnt == 1) {
		memfree(img);
		*cnt = 0;
	}
}

int main(void)
{
	imagine img, img_temp;
	int x1, x2, y1, y2, angle, cnt = 0;
	char *num, *command, command_line[200], *ascii_var, *num_out;
	while (1) {
		fgets(command_line, 200, stdin);
		command = strtok(command_line, "\n ");
		if (strcmp(command, "LOAD") == 0) {
			num = strtok(NULL, "\n");
			cnt_check(img, &cnt);
			img_temp = read(num, &cnt);
			if (img_temp.imagine_arr) {
				img = img_temp;
				select_all(img, &x1, &y1, &x2, &y2);
			}
		}
		if (strcmp(command, "SELECT") == 0) {
			if (cnt > 0)
				select_function(img, &x1, &y1, &x2, &y2);
			cnt_function(&cnt);
		}
		if (strcmp(command, "EXIT") == 0) {
			if (cnt == 0)
				printf("No image loaded\n");
			else
				memfree(img);
			return 0;
		}
		if (strcmp(command, "SEPIA") == 0) {
			if (cnt > 0)
				img = sepia_function(img, x1, y1, x2, y2);
			cnt_function(&cnt);
		}
		if (strcmp(command, "GRAYSCALE") == 0) {
			if (cnt > 0)
				img = grayscale_function(img, x1, y1, x2, y2);
			cnt_function(&cnt);
		}
		if (strcmp(command, "CROP") == 0) {
			if (cnt > 0) {
				img = crop_function(img, x1, y1, x2, y2);
				select_all(img, &x1, &y1, &x2, &y2);
			}
			cnt_function(&cnt);
		}
		if (strcmp(command, "ROTATE") == 0) {
			angle = atoi(strtok(NULL, "\n"));
			if (cnt > 0)
				img = decides_what_type_of_rotation(img, angle,
													&x1, &y1, &x2, &y2);
			cnt_function(&cnt);
		}
		if (strcmp(command, "SAVE") == 0) {
			cnt_function(&cnt);
			if (cnt > 0) {//in cont verific daca mai este scris cevs
				num_out = strtok(NULL, "\n ");//dupa numele fisierului
				if (num_out && num_out[strlen(num_out) - 1] == '\n')
					num_out[strlen(num_out) - 1] = '\0';
				ascii_var = strtok(NULL, "\n ");
					if (ascii_var && ascii_var[strlen(ascii_var) - 1] == '\n')
						ascii_var[strlen(ascii_var) - 1] = '\0';
					save_function(img, num_out, ascii_var);
						printf("Saved %s\n", num_out);
			}
		}
		if (!(strcmp(command, "ROTATE") == 0 || strcmp(command, "CROP") == 0 ||
			  strcmp(command, "GRAYSCALE") == 0 || strcmp(command, "SEPIA")
			== 0 || strcmp(command, "EXIT") == 0 || strcmp(command, "SELECT")
			== 0 || strcmp(command, "SELECT ALL") == 0 ||
			strcmp(command, "LOAD") == 0 || strcmp(command, "SAVE") == 0)) {
			printf("Invalid command\n");
			continue;
		}
	command[0] = '\0';
	}
memfree(img);
return 0;
}

