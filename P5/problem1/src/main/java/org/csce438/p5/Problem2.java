package org.csce438.p5;

import java.io.IOException;
import java.util.*;

import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.*;
import org.apache.hadoop.mapred.*;

public class Problem2 {
  public static class Map extends MapReduceBase implements Mapper<LongWritable, Text, Text, IntWritable> {
    // Setup static value for 1 and reusable Text
    private final static IntWritable one = new IntWritable(1);
    private final static Text countWord = new Text("__RUNNING_TOTAL__");
    private Text word = new Text();

    // What the lines represent for clarification
    public static final int DATA_TIME = 0;
    public static final int DATA_USER = 1;
    public static final int DATA_TEXT = 2;

    // Implement map
    @Override
    public void map(LongWritable key, Text value, OutputCollector<Text, IntWritable> output, Reporter reporter) throws IOException {
      // Get full chunk (delimited by "\n\n" by `conf.set("textinputformat.record.delimiter", "\n\n")`)
      String dataElement = value.toString();
      // Separate into component lines
      String[] fullData = dataElement.split("\n");
      // Handle first line case
      int dataOffset = 0;
      if (fullData.length > 3) {
        ++dataOffset;
      }
      // Get columns
      String[] timeData = fullData[DATA_TIME + dataOffset].split("\\s");
      try {
        // Get but don't collect hour yet
        String hmsData = timeData[2];
        String hourData = hmsData.substring(0, 2);
        word.set(hourData);
      } catch (ArrayIndexOutOfBoundsException e) {
        System.err.println("\n================================================");
        System.err.println("[Array out of bounds]:");
        System.err.println("timeData: " + Arrays.toString(timeData));
        System.err.println("Full Chunk:\n" + dataElement);
        System.err.println("================================================\n");
      }
      // No see if sleep is present in any capacity in the tweet text
      String textData = fullData[DATA_TEXT + dataOffset];
      String lowerTextData = textData.toLowerCase();
      if (lowerTextData.contains("sleep")) {
        System.out.println("\n================================================");
        System.out.println("Sleep Found");
        System.out.println("Chunk:\n" + dataElement);
        System.out.println("================================================\n");
        output.collect(word, one);
      }
      // Set running total
      output.collect(countWord, one);
    }
  }

  // Uses Reducer implementation from WordCount example
  public static class Reduce extends MapReduceBase implements Reducer<Text, IntWritable, Text, IntWritable> {

    @Override
    public void reduce(Text key, Iterator<IntWritable> values, OutputCollector<Text, IntWritable> output, Reporter reporter) 
      throws IOException {
      int sum = 0;
      while (values.hasNext()) {
        sum += values.next().get();
      }
      output.collect(key, new IntWritable(sum));
    }
  }

  public static void main(String[] args) throws Exception {

    JobConf conf = new JobConf(Problem2.class);
    conf.setJobName("problem2");

    conf.setOutputKeyClass(Text.class);
    conf.setOutputValueClass(IntWritable.class);
    
    conf.setMapperClass(Map.class);
    conf.setCombinerClass(Reduce.class);
    conf.setReducerClass(Reduce.class);

    // This seems to still be using '\n'
    // conf.set("mapreduce.input.keyvaluelinerecordreader.key.value.separator", "\n\n");
    // This one uses "\n\n"
    conf.set("textinputformat.record.delimiter", "\n\n");
    conf.setInputFormat(TextInputFormat.class);
    conf.setOutputFormat(TextOutputFormat.class);

    FileInputFormat.setInputPaths(conf, new Path(args[0]));
    FileOutputFormat.setOutputPath(conf, new Path(args[1]));

    JobClient.runJob(conf);
  }
}