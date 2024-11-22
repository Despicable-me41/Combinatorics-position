#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstring>
#include <ctime>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>

/*
  Knowing this, this software's version is very likely temporary. The algorithm is prone to change and modification.
*/

struct student_t {
    std::string student_name;
    float socialPotential; 
    std::string smallname;
    float priority; 
};
struct classroom_t{
    std::string configurationName; 
    std::vector<student_t *> student;
    int width; int height; 
    int configuration_n;
    float score; 

    long hashposition; 
};
long hashposition(const std::string& configuration){
    long hash = 0; 
    int order = 1;
    for(auto& chr : configuration){
        if(chr == '-')continue; 
        hash += chr * ((2) * order);
        order++;
    }
    return hash;
}
std::vector<std::string> breakchain(const std::string& chain){
    std::vector<std::string> broken_chain; 
    int i = 0; 

    while(i < chain.size()){
        std::string chainmember = "";
        while(chain[i] != '-' && i < chain.size()){
            chainmember.push_back(chain[i]);
            i++;
        }

        broken_chain.push_back(chainmember); 
        i++;
    }

    return broken_chain; 
}
void calculateScore(classroom_t& room1){
    const int roomContainables = room1.student.size();
    float score = 0.0f;

    for(int i = 0; i < roomContainables; i++){
        float speech_potential = 0;
        for(int j = 0; j < roomContainables; j++){
            if(i == j)continue; 
            struct vec2 {float x; float y;};

            vec2 student1 = {(float) (i % room1.width), (float) ((i / room1.width) % room1.height)};
            vec2 student2 = {(float) (j % room1.width), (float) ((j / room1.width) % room1.height)};

            float dist = (sqrtf(powf(student1.x - student2.x, 2) + powf(student1.y - student2.y,2)));
            speech_potential += ((room1.student[i]->socialPotential * (room1.student[i]->priority+1)) * (room1.student[j]->socialPotential * (room1.student[j]->priority + 1))) / powf(dist, 2.0f); 
        }

        score += room1.student[i]->socialPotential / speech_potential;
    }

    room1.score = score;
}
void generatePosition(std::vector<student_t>& std, classroom_t& cls, int width, int height){
    srand(time(0));
    std::vector<int> selectedPositionns; 
    std::string configuration;

    cls.configurationName = "";
    cls.width = width; 
    cls.height = height;
    cls.student.clear();

    for(int k = 0; k < std.size(); k++){
        int selectedRandom = rand() % std.size();

        auto isthere = [&](const int& l){
            for(auto& pos :selectedPositionns){
                if(pos == l)return true;
            }

            return false;
        };
        
        while(isthere(selectedRandom)){
            srand(time(0));
            selectedRandom = rand() % std.size();
        }

        cls.student.push_back(&std[selectedRandom]);
    
        selectedPositionns.push_back(selectedRandom);

        configuration += std[selectedRandom].smallname + '-';
    }

    cls.configurationName = configuration;

    long hash = hashposition(configuration);

    cls.hashposition = hash; 
}
std::vector<student_t> tokenize_file(std::ifstream& infile){
    std::vector<student_t> students; 
    char index = infile.get();

    std::string *copiedFile = new std::string();
    while(index != EOF){
        copiedFile->push_back(index);
        index = infile.get(); 
    }
    
    const int fsize = copiedFile->size(); 
    for(int i = 0; i < fsize; i++){
        if(copiedFile->at(i) == '{'){
            student_t nstudent; 
            do{
                i++;
                std::string sname = "", ssmallname = "", ssocial_p = "", spriority = ""; 
                while(copiedFile->at(i) != ',')sname.push_back(copiedFile->at(i++));
                i++;
                while(copiedFile->at(i) != ',')ssmallname.push_back(copiedFile->at(i++));
                i++;
                while(copiedFile->at(i) != ',')ssocial_p.push_back(copiedFile->at(i++));  
                i++;             
                while(copiedFile->at(i) != '}')spriority.push_back(copiedFile->at(i++));
                
                ssmallname.erase(ssmallname.begin());

                for(int i = 0; i < ssocial_p.size(); i++){
                    if(!isdigit(ssocial_p[i]) && ssocial_p[i] != '.')ssocial_p.erase(ssocial_p.begin() + i);
                }
                
                for(int i = 0; i < spriority.size(); i++){
                    if(!isdigit(spriority[i]) && spriority[i] != '.')spriority.erase(spriority.begin() + i);
                }
            
                nstudent.student_name = sname; 
                nstudent.smallname = ssmallname; 
                nstudent.socialPotential = std::stof(ssocial_p);
                nstudent.priority = std::stof(spriority);
                
            }while((copiedFile->at(i) != '}'));

            students.push_back(nstudent);
        }
    }

    delete copiedFile;
    return students;
}


int main(int argc, char **argv){
    
    if(!strcmp(argv[1], "-help")){
       std::cout << "The algorithm is a software that computes and evaluates different positions. You interact with the software through a CLI interface.\n";
       std::cout << "The software is open-source and free-to-use, you add a graphics interface to make the interaction process throuogh.\n\n";
       std::cout << "Normal mode: ./aglo [<simname>:text] [<width>:integer] [<height>:integer] [<infile>:file_path] [<n_simulations>:integer]\n";
       std::cout << "Evaluation mode: ./aglo -evaluate [<width>:integer] [<height>:integer] [<infile>:file_path] [<configuration>:text]\n";
       std::cout << "Evaluation mode is an algorithm state where a classroom set is given through the \'configuration\' argument. The algorithm evaluates that set and position to score that position.\n\n";
       return 0;
    }

    if(!strcmp(argv[1], "-evaluate")){
        int width= 0, height = 0;

        width = std::stoi(argv[2]);
        height = std::stoi(argv[3]);

        std::ifstream infile; 
        
        infile.open(argv[4], std::ios_base::in);

        std::vector<student_t> students = tokenize_file(infile); 
        classroom_t set_classroom; 

        set_classroom.configurationName = argv[5];

        infile.close();

        set_classroom.width = width; 
        set_classroom.height = height; 

        std::vector<std::string> smallname_collection = breakchain(set_classroom.configurationName); 

        for(auto smlnames : smallname_collection){
            student_t *stn = NULL;

            auto lowerstr = [&] (const std::string& ls){
                std::string a; 
                for(auto& chr : ls)a.push_back(tolower(chr));

                return a; 
            };

            for(auto stnds : students){
                if(!strcmp(lowerstr(stnds.smallname).c_str(), lowerstr(smlnames).c_str())) stn = &stnds;  
            }

            if(stn == NULL){
                std::cout << "Invalid inputed name not matching any existings small names.\n";
                std::cout << "Reached small name: " << smlnames << '\n';
                return 1;
            }

            set_classroom.student.push_back(stn);
        }

        long hash = hashposition(set_classroom.configurationName); 

        set_classroom.hashposition = hash; 
        calculateScore(set_classroom);

        std::cout << "Configuration " << "(Provided)" 
        << ": " << "Score: " << set_classroom.score 
        << " Log score (in Rbs): " << log10(set_classroom.score) * 10 << " - " << set_classroom.hashposition << " ; " << set_classroom.configurationName << "\n";

        for(auto& st : set_classroom.student)
            delete st; 

        return 0;
    }


    // Normal mode.
    int width, height;
    std::string simname; 

    simname = argv[1];
    width = std::stoi(argv[2]);
    height = std::stoi(argv[3]);
   
    std::vector<student_t> students;

    std::ifstream infile;
    infile.open(argv[4], std::ios_base::in);
    students = tokenize_file(infile);
    infile.close();

    int k = 0;
    k = std::stoi(argv[5]);
    std::vector<classroom_t> simulatedEntries; 

    for(int i = 0; i < k; i++){
        classroom_t new_entry; 
        generatePosition(students, new_entry, width, height);
        
        auto checkEntry = [&](const classroom_t& entry1){
            for(auto& sim_entry :simulatedEntries){
                if(entry1.hashposition == sim_entry.hashposition)return true;
            }
            return false;
        };

        while(checkEntry(new_entry)) generatePosition(students, new_entry, width,height);
        calculateScore(new_entry);

        std::cout << "Configuration " << (i+1) 
        << ": " << " Score: " << new_entry.score 
        << " Log score (in Rbs): " << log10(new_entry.score) * 10 << " - " << new_entry.hashposition << " ; " << new_entry.configurationName << "\n";
        new_entry.configuration_n = i+1;
        simulatedEntries.push_back(new_entry);
    }
   
    // Better select another sorting algorithm than bubblesort. 
    const int entriesL = simulatedEntries.size();
    for(int k = 0; k < entriesL - 1; k++){ // Optimize the bubblesort to get a time complexity of n(n-1) instead of n^2
        for(int i = 0; i < entriesL; i++){
          int j = i+1;
          if(!(j < entriesL))break;

          if(simulatedEntries[i].score < simulatedEntries[j].score)std::swap(simulatedEntries[i], simulatedEntries[j]);
       }
    }

    std::cout << "Ordered up entries: \n";
    for(auto& entry : simulatedEntries){
        std::cout << "Configuration " << (entry.configuration_n) 
        << ": " << " Score: " << entry.score 
        << " Log score (in Rbs): " << log10(entry.score) * 10 << " - " << entry.hashposition << " ; " << entry.configurationName << "\n";
    } 
    
    std::ofstream out_file;
    out_file.open(simname + ".alrs", std::ios_base::out);

    out_file << "Virtual classroom resolution (width, height): " << width << ", " << height << "\n"; 
    
    for(auto& pls : students){
        out_file << pls.student_name << ": delta_s: " << pls.socialPotential << " small_name: " << pls.smallname << "\n";
    }

    std::cout << "\n";

    for(auto& entry : simulatedEntries){
        out_file << "Configuration " << (entry.configuration_n) 
        << ": " << " Score: " << entry.score 
        << " Log score (in Rbs): " << log10(entry.score) * 10 << " - " << entry.hashposition << " ; " << entry.configurationName << "\n";
    }

    out_file.close();

    return 0;
}