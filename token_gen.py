tokens = "` # ( ) [ ] { } : ; \\n \\\" \\\' \\\\\\\" \\\\\\\' , . ? ++ --".split(" ")
operators = "~ ! @ $ % ^ & && * ** - + | || < << > >> / // =".split(" ")
print("{",end="")
enclose = lambda x: "\"%s\""%x
print(",".join(map(enclose, tokens)),end=",")
print(",".join(map(enclose, operators)),end=",")
print(",".join(map(enclose, [a+"=" for a in operators])),end="}\n")