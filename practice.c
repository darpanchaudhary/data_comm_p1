#include <unistd.h>             /*  for ssize_t data type  */

#include <sys/socket.h>
#include <errno.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>


char type1_val_buf[sizeof(unsigned int)];
char byte_read;
char type0_amount;
unsigned int type1_amount;
char type0_int[2];
char type1_amount_buf[3];
unsigned int count = 0;
bool is_read_type = true;
unsigned int last_type = 0;
bool is_read_amount = true;
bool end = false;



unsigned int convert_type0_int(char* type0_buf){
	unsigned int val1 = type0_buf[0];
	unsigned int val2 = type0_buf[1];
	return ((val1<<8) | val2);
}

unsigned int convert_type1_int(char* type1_buf){
	unsigned int val1 = type1_buf[0] - '0';
	unsigned int val2 = type1_buf[1] - '0';
	unsigned int val3 = type1_buf[2] - '0';
	// printf("%d%d%d\n", val1, val2, val3);

	unsigned int amount = val1*10000+val2*100+val3;
	// printf("++%d\n",amount);
	return amount;

}

int main()
{
	char *input_filename = "test_file_1.dms";
	char *output_filename = "out";
	FILE *in_ptr = fopen(input_filename, "rb");
	if (in_ptr == NULL) {
    	perror("Failed: to open the input_file");
    	return 1;
	}
	FILE *out_ptr = fopen(output_filename, "wb");
	if (out_ptr == NULL) {
    	perror("Failed: to open the output_file");
    	return 1;
	}
	printf("reading file successful\n");

	CHECK_AGAIN:
	while(!end){	
		if(is_read_type){
			if(fread(&byte_read, sizeof(byte_read), 1, in_ptr)!=1){
				end = true;
				goto CHECK_AGAIN;
				
			}
			START_READ:
			printf("Read Type: %d ", byte_read);

			char type_write = (byte_read+1)%2;
			fwrite(&type_write, 1, 1, out_ptr);
			printf("Write Type: %d ", type_write);
			last_type = byte_read;
			is_read_type = false;

		}
		else if(is_read_amount){

			if (last_type == 0){
				if(fread(&byte_read, sizeof(byte_read), 1, in_ptr)!=1){
					end = true;
					goto CHECK_AGAIN;
				}
				type0_amount = byte_read;
				printf("read amount: %d ", type0_amount);
				unsigned int temp_amount;
				unsigned int temp_type0_amount = type0_amount;
				printf("write amount: ");
				for (unsigned int i=0; i<3; i++) {
				// Making amount as 3 byte ascii
					temp_amount= temp_type0_amount/pow(10, 2-i)+'0';
					temp_type0_amount = temp_type0_amount % (int)pow(10,2-i);
					printf("%c", temp_amount);
					fwrite(&temp_amount, 1, 1, out_ptr);
				}
				printf(" ");
			}
			else if(last_type == 1){
				if(fread(&type1_amount_buf, sizeof(type1_amount_buf), 1, in_ptr)!=1){
					end = false;
					goto CHECK_AGAIN;
				}
				type1_amount = convert_type1_int(type1_amount_buf);
				printf("read amount: %d ", type1_amount);
				printf("write amount: %d ", (type1_amount & 255));
				// convert 4 byte int to 1 byte char.
				fwrite(&type1_amount, 1, 1, out_ptr);
			}

			is_read_amount = false;
		}
		else if(last_type==0){
			for(unsigned int i=0; i<type0_amount; i++) {
				if(fread(&type0_int, sizeof(type0_int), 1, in_ptr)!=1){
					end = true;
					goto CHECK_AGAIN;
				}
				// printf("\n%d %d \n", type0_int[0], type0_int[1]);
				unsigned int val = convert_type0_int(type0_int);
				printf("Read Number: %u ", val);
				printf("write Number: ");

				unsigned int temp_a;
				for(int i=4; i>=0; i--){
					temp_a = val/pow(10,i)+'0';
					val = val % (int)pow(10,i);
					printf("%c",temp_a);
					fwrite(&temp_a, 1, 1, out_ptr);
				}
				char comma=',';
				if(i<(type0_amount-1)){
					printf(",");
					fwrite(&comma, 1, 1, out_ptr);
				}
				printf(" ");

				
			}
			printf("\n");
			is_read_type = true;
			is_read_amount = true;
		}

		else if(last_type==1){
			for(unsigned int i=0; i<type1_amount; i++) {
				unsigned int temp_val = 0;
			 	if(fread(&byte_read, sizeof(byte_read), 1, in_ptr)!=1){
			 		end = true;
					goto CHECK_AGAIN;
			 	}
				if (byte_read ==0 || byte_read ==1){
					if(i != (type1_amount-1)){
						perror("type1 read values != type1 amount");
    					return 1;
					}

					
				}
				else {
					unsigned int num = 0;
					printf(" Read Number: ");
					while(byte_read!=44){
						if (byte_read ==0 || byte_read ==1){
							break;
						}

						printf("%c", byte_read);
						num = num*10 + (byte_read-'0');
						// printf("\n---%d--\n", num);
						if(fread(&byte_read, sizeof(byte_read), 1, in_ptr)!=1){
			 				end = true;
					        goto CHECK_AGAIN;
			 			}

					}
					if (end){
						break;
					}
					fwrite(&num, 2, 1, out_ptr);
					printf(" Write Number: %d", num);
					if (byte_read ==0 || byte_read ==1){
						printf("Read Type: %d ", byte_read);
						printf("Write Type: %d ", ((byte_read+1)%2));
						last_type = byte_read;
						is_read_amount = true;
						printf("\n");
						// goto START_READ;
						
						// print("Read Type: ")
						 break;
					}
					// printf("\n %c\n", byte_read);
					

					// print()
					// break;
				}
				
			}

		}
		


	}
	


}