main() 
{ 
  int i,j,k,N; 
  float soma; 
  float A[15]; 
  float B[5],X[5]; 
  printf("Número de equações: "); 
  scanf("%d",&N); 
  i = 1; 
  j = 0; 
  while (i <= N)  
  { 
    j = j + i; 
    i = i + 1; 
  } 
  printf("Coeficientes da matriz A (por linha)\n"); 
  i = 0; 
  while (i < j) 
  { 
    scanf("%f",&A[i]); 
      i = i + 1; 
  } 
  printf("Vetor B: "); 
  i = 0; 
  while (i < N)  
  { 
    scanf("%f",&B[i]); 
    i = i + 1; 
  } 
  // Determinação do vetor solução 
  X[0] = B[0]/A[0]; 
  i = 1; 
  k = 1; 
  while (i < N)  
  { 
    soma = 0; 
    j = 0; 
    while (j < i) 
    { 
      soma = soma + A[k]*X[j]; 
      j = j + 1; 
      k = k + 1; 
    } 
    X[i] = (B[i]-soma)/A[k]; 
    i = i + 1; 
    k = k + 1; 
  } 
 
  printf("Vetor solução:\n"); 
  i = 0; 
  while (i < N)  
  { 
    printf("%f\n",X[i]); 
    i = i + 1; 
  } 
}