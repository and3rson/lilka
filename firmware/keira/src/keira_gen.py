#!/usr/bin/python
import os, sys

# Get templates path from env
templates_path = os.getenv("KEIRA_TEMPLATES_PATH")
source_path = os.getenv("KEIRA_SOURCE_PATH")

if templates_path == None:
    templates_path = "templates"
if source_path == None:
    source_path = ""

template_filenames = os.listdir(templates_path)

def help():
    what_could_gen = []
    for filename in template_filenames:
        if os.path.isdir(filename):
            what_could_gen.append(filename)
        print("\t{} {}Name".format(filename, filename))



if len(sys.argv)< 3:
    print("Wrong count of parameters specified.\n Generators:")
    help()

elif len(sys.argv) == 3:
    what_to = sys.argv[1]
    name = sys.argv[2]
    path_to_template = templates_path +"/"+what_to
    if not os.path.isdir(path_to_template):
        print("Generator {} doesn't exist. ")
        help()
    else:
        for filename in os.listdir(path_to_template):
            new_filename = source_path+"/"+ what_to+"s/"+filename.replace("$NAME$", name)
            if (os.path.exists(new_filename)):
                print("Can't generate source file cuz it allready exists")
                sys.exit()            
            template_file = open(path_to_template+"/"+filename)
            generated_file = open(new_filename, "w")
            template_file_lines = template_file.readlines()
            generated_file_lines = []

            for line in template_file_lines:
                generated_file_lines.append(line.replace("$NAME$", name))
            generated_file.writelines(generated_file_lines)
            generated_file.close()
            template_file.close()

