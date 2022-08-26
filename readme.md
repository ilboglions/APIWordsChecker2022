APIWordsChecker2022 by Matteo Boglioni
Computer Engineering Student at Politecnico di Milano https://www.polimi.it/
Final Project of Algorithms and Principles of Informatics (API) Course 2022 hold by Prof. Alessandro Barenghi
Grade: 30/30 cum Laude.

The specs of this project are available in the file "Task.pdf" in this repository. 

There are 6 Versions of this project, the main reason for that is the different use from one to another of the main data structure.
During the developing I had the chance to understand the trade-offs of each data structure trying to find the best compromise. 
    -Mk1 Version is based on a Trie, incredibly fast in the search of a word in the dictionary but exceptionally space-consuming.
    -Mk2 is based on RB Trees, to make faster confronts and insertions in the tree I implemented an encoding of a string in a 64bit unsigned integer.
     Unluckily the encoding and decoding of strings was too much frequent to be time competitive.
    -Mk3 brings the strings inside the RB Tree. The program becomes faster but still slow for our test-cases. The reason is that having a valid flag on every 
     word in the tree makes the filtering during the game very time-consuming. To find still compatible words we have to consider the full tree (included the words already excluded).
    -Mk4: to fix the problems of Mk3 version I created a temporary BST that keeps the still valids words. The program is faster but a simple BST can be very  
     unbalanced making the insertion very time-consuming.
    -Mk5: Maybe the solution could be making 2 different RB-trees, one with all the words, the second (temporary tree) with the words still valids. This kind 
     of approach could be interesting because can be really fast, the downside is that having 2 RB-Trees is extremely space-consuming (due to padding inside the data structure declaration), it exceeded the memory limits given by the testing platform.
    -Mk6. the final version of the project uses the RB-Tree for the global dictionary of words and a simple list for the words still compatible in the game.
     This makes the program efficient and light. The only drawback is that when the program needs to print all the still available words ("+stampa_filtrate") we need to sort the list (implemented by Merge-Sort). Luckily this kind of situation is very unusual in the tests.

The folder Generated_Test_Cases has some Test_Cases generated by me to help other students to test their project giving also the correct expected output.


For any kind of question please contact me at boglioni.matteo@gmail.com 