main()
{
int D,B,k,quoc,resto;
int num[20];
printf("Entre com um inteiro e uma base: ");
scanf("%d %d",&D,&B);
quoc = D;
k = 0;
while (quoc >= B)
{
resto = quoc % B;
quoc = quoc / B;
num[k] = resto;
k = k + 1;
}
num[k] = quoc;
printf("Representação de %d na base %d: ",D,B);
while (k >= 0)
{
printf("%d",num[k]);
k = k - 1;
}
}