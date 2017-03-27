class GraphSmoother:
    def __init__(self):
        pass

    def smooth_graph(self, width, binary_points):
        """
        Takes in a binary list and converts it into a smooth line
        for analytics graphing.

        E.g.
        width = [0,0,1,0,1,1,1,1,1,0,1,1,1,0,1,0,1,0,0]
        search_margin = 1

        return [0,1,1,2,2,3,3,3,2,2,2,3,2,2,1,2,1,1,0]
        """
        smooth_points = []
        x = 0
        while x < len(binary_points):
            selected_points = []
            if x < width:
                selected_points = binary_points[0:x+width+1]
            elif x > len(binary_points) - width:
                selected_points = binary_points[x-2:]
            else:
                selected_points = binary_points[x-width:x+width+1]
            if len(selected_points) > 0:
                smooth_points.append(sum(selected_points))
            x+=1
        return smooth_points

if __name__ == '__main__':
    print('Running tests for: graph-smoother.py')

    ## Instantiate the class
    smoother = GraphSmoother()

    ## Test data
    blob = [0,0,1,0,1,1,1,1,1,0,1,1,1,0,1,0,1,0,0]

    ## Do the thing and print it
    blob = smoother.smooth_graph(2, blob)
    print(blob)
