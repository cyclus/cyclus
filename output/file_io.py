########################################################################

def list2file(outputFile, *args):
        
        """
        Writes Python lists in colomns into the output text file.
        inputs:
        output file
        ColomnLabel1, List1, ColomnLabel2, List2...
        """
        
        Text = []

        length = 0
        for i in range(1, len(args), 2):
                length = max(length, len(args[i]))
                

        #Tests that args is a string (the column label) and then a list (the data) etc
        for i in range(1, len(args), 2):

                if type(args[i-1]) != type(''):
                        raise Exception, "Wrong arguments."

                if type(args[i]) != type([]):
                        raise Exception, "Wrong arguments."

        
                
        #Add the first line with the column names       
        for i in range(0, len(args), 2):

                if type(args[i]) != type(''):
                        raise Exception, "No column name."
                else:
                        Text.append(args[i] + '\t')

        Text.append('\n')

        #Add the data
        for raw in range(length):       

                for i in range(1, len(args), 2):

                        if raw >= len(args[i]):
                                Text.append('\t')

                        else:
                                Text.append(str(args[i][raw])+'\t')

                Text.append('\n')

        
        f = open(outputFile, 'w')
        f.writelines(Text)
        f.close()
        
#list2file('test.txt', 'col1', [4,4,2,3,5,7,5], 'col2', [4,4,2,3,5,7,5,2])




def dic2file(outputFile, D):
        
        """
        Writes a Python dictioinary into the output text file.
        inputs:
        output file
        Dic = {label1: [data1], ...}
        """
        
        #Tests that the dic has the right frame
        for key in D.keys():

                if type(key) not in (type(''), type(0)):
                        raise Exception, "Wrong key type. Must be str or int."

                if type(D[key]) != type([]):
                        raise Exception, "Wrong data type. Must be a lsit."



        Text = []

        length = 0
        for data in D.values():
                length = max(length, len(data))

                
        #Add the first line with the column names       
        for key in sorted(D.keys()):

                Text.append(str(key) + '\t')

        Text.append('\n')

        #Add the data
        for raw in range(length):       

                for key in sorted(D.keys()):

                        if raw >= len(D[key]):
                                Text.append('\t')

                        else:
                                Text.append(str(D[key][raw])+'\t')

                Text.append('\n')

        
        f = open(outputFile, 'w')
        f.writelines(Text)
        f.close()

#dic2file('test.txt', {'sdrg':[0,4,2,8,28,5,1,5], 'sdfrg':[1,2,58,21,5,5]})
