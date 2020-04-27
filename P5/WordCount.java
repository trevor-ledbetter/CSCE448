

package org.csce438.p5;

import java.io.IOException;
import java.util.*;

import org.apache.hadoop.fs.Path;
import org.apache.hadoop.conf.*;
import org.apache.hadoop.io.*;
import org.apache.hadoop.mapred.*;
import org.apache.hadoop.util.*;

import java.text.SimpleDateFormat;
import java.text.ParseException;
import java.lang.Exception;

public class WordCount {
  public static class Map extends MapReduceBase implements Mapper<LongWritable, Text, Text, IntWritable> {
    private final static IntWritable one = new IntWritable(1);
    private Text word = new Text();

    @Override
    public void map(LongWritable key, Text value, OutputCollector<Text, IntWritable> output, Reporter reporter)
        throws IOException {
      String line = value.toString();
      StringTokenizer tokenizer = new StringTokenizer(line);
      
      /*for(int i=0; i<1; i++){
        word.set(Integer.toString(0));
        output.collect(word, one);
      }
      }
      }*/
      //Finds the hour that the tweet was posted. 
      while(tokenizer.hasMoreTokens()) {
        String word_string = tokenizer.nextToken();
        SimpleDateFormat format = new java.text.SimpleDateFormat("HH:mm:ss");
        try{
          format.parse(word_string);
          //word_string is a timestamp, otherwise exception would be caught below
          int hour = Integer.parseInt( word_string.substring(0,2) ); //the hour
          
          word.set(Integer.toString(hour));
          output.collect(word, one);
          //once the timestamp is found, we break for effeciency, 
          //and because there might be valid timestamps in the tweet itself.
          break;
        }
        catch(ParseException e) {
          //Word does not fit the format, do nothing
          //word.set(Integer.toString(2));
          //output.collect(word, one);
          continue;
        }
      }
      


      /*//Skip over "U", Username (which may contain sleep which is why we are explicity skipping), and "W"
      for(int i=0; i<3; i++){
        if(tokenizer.hasMoreTokens()){
          tokenizer.nextToken();
        }
      }

      //Finds "sleep" in the body of the tweet, if successful output the hour of the post
      while(tokenizer.hasMoreTokens()) {
        String word_string = tokenizer.nextToken();
        word.set(Integer.toString(3));
        output.collect(word, one);


        String word_string = tokenizer.nextToken();
        //output.collect(word, one);
        //Case insensitive contains
        if( word_string.toLowerCase().contains("sleep") ){
          //Sucessfully found sleep in the body of a tweet
          //return word (the hour of the tweet found above)
          output.collect(word, one);
        }
        break;
      }*/
    }
  }

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

    JobConf conf = new JobConf(WordCount.class);
    conf.setJobName("wordcount");

    conf.setOutputKeyClass(Text.class);
    conf.setOutputValueClass(IntWritable.class);
    
    conf.setMapperClass(Map.class);
    conf.setCombinerClass(Reduce.class);
    conf.setReducerClass(Reduce.class);

    //new
    conf.set("mapreduce.input.keyvaluelinerecordreader.key.value.separator", "\n\n");
    conf.setInputFormat(TextInputFormat.class);
    conf.setOutputFormat(TextOutputFormat.class);

    FileInputFormat.setInputPaths(conf, new Path(args[0]));
    FileOutputFormat.setOutputPath(conf, new Path(args[1]));

    JobClient.runJob(conf);
  }
}