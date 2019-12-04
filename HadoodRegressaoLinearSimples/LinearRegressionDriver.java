package br.ufsc.rlm;

import java.io.IOException;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.DoubleWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapred.FileInputFormat;
import org.apache.hadoop.mapred.FileOutputFormat;
import org.apache.hadoop.mapred.JobClient;
import org.apache.hadoop.mapred.JobConf;
import org.apache.hadoop.mapred.TextInputFormat;
import org.apache.hadoop.mapred.TextOutputFormat;

public class LinearRegressionDriver {

    public static void main(String[] args) throws IOException {
        JobClient jobClient = new JobClient();
        // Create a configuration object for the job
        JobConf jobConfiguration = new JobConf(LinearRegressionDriver.class);

        // Set a name of the Job
        jobConfiguration.setJobName("LinearRegressionGeoData");

        // Specify data type of output key and value
        jobConfiguration.setOutputKeyClass(LongWritable.class);
        jobConfiguration.setOutputValueClass(DoubleWritable.class);

        // Specify names of Mapper and Reducer Class
        jobConfiguration.setMapperClass(LinearRegressionMapper.class);
        jobConfiguration.setReducerClass(LinearRegressionReducer.class);

        // Specify formats of the data type of Input and output
        jobConfiguration.setInputFormat(TextInputFormat.class);
        jobConfiguration.setOutputFormat(TextOutputFormat.class);

        // Set input and output directories using command line arguments, 
        //   arg[0] = name of input directory on HDFS, and arg[1] = name of
        //   output directory to be created to store the output file.
        FileInputFormat.setInputPaths(jobConfiguration, new Path(args[0]));
        FileOutputFormat.setOutputPath(jobConfiguration, new Path(args[1]));

        jobClient.setConf(jobConfiguration);
        JobClient.runJob(jobConfiguration);
    }
}
