import unittest
import subprocess

def get_usrs():
    proc = subprocess.run([
        "./build/callclusterClang", 
        "./test-projects/simple/build",
        "--functions",    
        ],
        capture_output=True,
        text=True
    )
    lines=str(proc.stdout).split("\n");
    return [ l.replace("function: ","") for l in lines if len(l)>0];

def flow_file_name(usr):
    return "test_flow/"+usr+".txt"

def getFlow(function_name):
    proc = subprocess.run([
        "./build/callclusterClang", 
        "./test-projects/simple/build",
        "--flow",
        function_name
        ],
        capture_output=True,
        text=True
    )
    return proc.stdout;

def save_flows():
    for usr in get_usrs():
        with open(flow_file_name(usr),"w") as file:
            flow = getFlow(usr)
            if flow is None:
                print("Flow for "+usr+" is None")
            else:
                file.write(flow)

def getFileText(function_name):
    data="";
    with open(flow_file_name(function_name), 'r') as file:
        data = file.read()
    return data

class TestFlow(unittest.TestCase):
    def test_flow(self):
        

        for usr in get_usrs():
            with(self.subTest(usr=usr)):
                self.assertEqual(getFileText(usr), getFlow(usr))

if __name__ == '__main__':
    unittest.main()