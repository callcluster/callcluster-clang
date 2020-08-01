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
/*
int hello_there() {
   hello();
   there(5, 6 + main());
   return 0;
}

int checkin_it(){
   int b = 1;
   if(b==0){
      hello();
      b=2;
      there(b,b);
   }
   for(int i=0;i<15;i++){
      hello();
      there(3,3);
      there(3,3);
      there(3,3);
      i;
      i;
      i;
      i;
      i;
      i;
      i;
      i;
      i;
      i;
   }
}

int for_five(){
   for(int i=0;i<5;i++){
      i;
   }
   for(int i=0;i<5;i++){
      i;
   }
   for(int i=0;i<5;i++){
      i;
   }
   for(int i=0;i<5;i++){
      i;
   }
   for(int i=0;i<5;i++){
      i;
   }
}

int while_while(){
   while(1){
      int i=0;
      1;
      2;
      3;
   }
   while(8){
      5;
   }
}

void run_if(){
   int b=4;
   if(b==0){
      hello();
      b=2;
      there(b,b);
   }else{
      hello();
   }
   hello();
   if(b==2)
      there(b,b);
   
   hello();
}
*/
void compound_compound(){
   hello();
   hello();
   hello();
   {
      hello();
      hello();
   }

   there(1,1);

   {
      hello();
      hello();
   }

   {
      hello();
      hello();
   }
}
