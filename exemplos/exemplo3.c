
main() {
    int n, i ;
    int primeiro, proximo, segundo;
    primeiro = 0;
    segundo = 1;
    i = 0;

    printf("Digite quantos termos de Fibonacci deseja imprimir: ");
    scanf("%d", &n);

    printf("Sequência de Fibonacci:\n");

    while (i < n) {
        if (i == 0) {
            printf("%d ", primeiro);
        } else { 
            if (i == 1) {
                printf("%d ", segundo); 
            }
             else {
                proximo = primeiro + segundo;
                printf("%d ", proximo);
                primeiro = segundo;
                segundo = proximo;
            }
        }
        
        i = i + 1;
    }

    printf("\n");
    
}
