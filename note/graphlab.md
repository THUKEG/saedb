#About using map reduce with graphlab (k-means)
#2013.3.25
#reference: https://group.google.com/forum/?fromgroups=#!topic/graphlabapi/YawqDQZ4Sec
Graphlab don't have an interface for a single "map_reduce_over_neighbors" call.
For k-means, first create a fully connected bipartite graph with data points on one side, and K centers on the other side.
Then implement an update function.
On *data points* side: on gather compute distance to each center, and on apply, store on the data point, the id of the closest center.
On *center* side: on gather computer the total of all data points assigned to the center, and on apply average the result.

Q: Use a *global variable* or a *vertex connecting to all vertex*:
A: Use a global variable:
    1.If the global variable is constant, or is deterministic, there are no problems.
    2.If not, then as long as it is always the output of some Graphlab "aggregate" operation.
      For instance: global_variable_result = graph.map_reduce_vertices(...)
      then, Grpahlab does ensure that all machines get the same value for global_variable_result



