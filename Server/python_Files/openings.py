import csv
with open('D:\ChessAI\Server\python_Files\openings_fen7.csv', mode ='r') as file:
    csvFile = csv.reader(file)
    output = "unordered_map<string, string> opennings = {"
    for lines in csvFile:
            output += "{\"" + lines[0] + "\"," + " \"" + lines[1] + "\"}, "
        
    output += "};"
    
    with open("output.txt", "w") as text_file:
        text_file.write(output)