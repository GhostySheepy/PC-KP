import os  
 
print(os. getcwd())
in_fol = "real job\\train\\unsup\\"
out_fol = "real job\\train\\unsup_str\\"
for root, dirs, files in os.walk(in_fol):
    
    for file in files:
        stripped_file = file.strip(".txt")
        stripped_file = stripped_file.split('_')[0]
        
        if(stripped_file.isdigit()):
            if(int(stripped_file) >= 4000 and int(stripped_file) <= 5000):
                ##print("is digit")
                os.rename(in_fol+file, out_fol+stripped_file+".txt")
                
        
        
            
