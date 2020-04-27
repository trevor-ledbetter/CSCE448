# P5: Apache MapReduce

## Notes on Container Creation with Access to Hadoop Overview:
Start up the container with this command (or similar, just needs to have the -p parts)
```
docker run -it --name hadoop_test --volume <local path where you store data and files>:/mnt/<docker folder
name where you will access local files> -p8088:8088 -p 50070:50070 sequenceiq/hadoop-docker:2.7.0 /etc/bootstrap.sh -bash
```
Note that this will require making a new container, as I think it can only setup ports on creation.

## Notes on Usage:
Since it is organized under the `org.csce438.p5` package, use this set of commands instead:
```
mkdir wordcount_classes

hadoop com.sun.tools.javac.Main -d wordcount_classes/ WordCount.java

jar -cvf wordcount.jar -C wordcount_classes/ .

hadoop jar wordcount.jar org.csce438.p5.WordCount /user/root/data /user/root/output
```

This lets you retrieve the output from hdfs to local:
```
hadoop fs -get /user/root/output/
```