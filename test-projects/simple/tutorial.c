#include <stdio.h>
int main() {
   printf("Hello, World!");
   return 0;
}



void there(int x, int y){
   main();
   there(main(), x);
}

void hello(){
   there(1 + main(), main() * 2);
}

int hello_there() {
   hello();
   there(5, 6 + main());
   return 0;
}
