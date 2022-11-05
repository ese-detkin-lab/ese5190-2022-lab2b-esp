## Lab 2b _ Worked with Amogh and saurabh.
# Pointers in C

Here in this code, we saved the address in a variable called 'var1' by using '&', in a pointer variable 'var2', by using '*' operator we can printout the value stored at an address
```
#include <stdio.h>

int main () {

   int  var1;
   int *var2; //Declare a pointer
   var2 = &var1 //Use '&' to get the address of a variable

   printf("Address of var1 variable: %x\n", &var1); //Address of Variable 'var1'
   printf("Address of var1 variable: %x\n", *var2); //Value of 'var1'

   return 0;
}
```


```
Address of var1 variable: bff5a400
Address of var2 variable: bff5a3f6
```

# Direct Register Access
Here We create two functions, first one to read, we pass the address as an argument, using the '*' operator we return the value stored at that address. For the second function
we pass the address and the value to be written at that address, using the same operator we put the value at that address
```
typedef          uint32_t   VALUE;
typedef volatile uint32_t* ADDRESS;


VALUE register_read(ADDRESS  address) {
    return *address;                                    //Reading and returing the register value
}

void register_write(ADDRESS address, VALUE value) {
    *address = value;                                   //Writing value to register
}
```



# Reading BOOT Button and Blinking Neopixel
To find which register to read, we need to find the base register, from the addressmap.h file, we find the SIO_BASE_u variable which is defined as 0xd0000000

![](https://github.com/saurabhparulekar24/ESE5190_LAB2B_PART01/blob/main/addressmap.png)

Now we need to find the offset, which we can find in the sio.h file, which is defined as 0x00000004

![sio](https://user-images.githubusercontent.com/57740824/200075208-7b9b2718-88c2-47d8-9dab-4269029effbc.png)

Therefore we need to read the address 0xd0000004, this won't give us the state of the BOOT, the value of this register will give us the state of all GPIOs,
to extract the state of BOOT button, we need to create a mask.
```
BOOT button is connected on GPIO21, hence the state of the BOOT button is the 21st bit from the right
for the mask, we shift 1 to left by 21, and 'and' or '&' it with the value of the register
a = register_read((ADDRESS)0xd0000004);
state_BOOT = !(1<<21 & a)
```

# Output of The Final Code
![ezgif com-gif-maker (1)](https://user-images.githubusercontent.com/57740824/200076783-8355db90-80b1-44a4-b93f-c1a6816d1a82.gif)
