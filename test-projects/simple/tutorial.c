#include <stdio.h>
int main() {
   printf("Hello, World!");
   return 0;
}


/*
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


void casi(){
   int b = 2;
   switch(b){
      b = 8;
      b < 8;
      case 1:
      b+=2;
      break;

      case 900:
      b+=2;
      break;

      case 0:
      case 2:
      b=5;
      break;

      case 3:
      case 4:
      case 5:
      b = 0;
      default:
      int q = 8;
      b = q;
   }
}

void normal_for(){
   int q=0;
   for(int i=0;i<10;i++){
      q+=i*i;
   }
}

void evil_for(){
   int q=0;
   for(int i=0;i<10;i++) q+=i*i;   
}

void broken_for(){
   int q=0;
   for(int i=0;i<10;i++){
      q+=i*i;
      if(q>8) break;
      if(i<3) continue;
      q=8;
   }
}
*/


/*
void normal_while(){
   int q=0;
   while (q<10)
   {
      q+=1;
   }
   q=8;
}
*/
/*
void evil_while(){
   int q=0;
   while(q<10) q+=1;
   q=8;
}
*/
/*
void broken_while(){
   int q = 0;
   while(q < 10){
      q += 1;
      if(q > 8) break;
      if(q < 3) continue;
      q = 8;
   }
   q = 9;
}

void while_with_unreachable_part(){
   int q = 0;
   while(q < 10){
      q += 1;
      if(q > 8){
         break;
      }else{
         return;
      }
      if(q < 3) continue;
      q = 8;
   }
   q = 9;
}

void while_with_return(){
   int q = 0;
   while(q < 10){
      q += 1;
      if(q > 8) return;
      if(q < 3) continue;
      q = 8;
   }
   q = 9;
}

void unreachable(){
   int q=0;
   return;
   q++;
   q++;
   q++;
}
*/

void gotos(){
   int q=0;
   start: q+=1;
   q=3;
   q+=1;
   if(q==0){
      goto end;
   }else{
      goto start;
   }
   end:return;
}
/*
void gotos_evil(){
   int q=0;
   start: q+=1;
   q=3;
   q+=1;
   if(q==0)
      goto end;
   else
      goto start;
   end:return;

   while(1) goto end;
}
*/