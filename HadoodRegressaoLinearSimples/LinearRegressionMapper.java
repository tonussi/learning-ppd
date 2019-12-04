package br.ufsc.rlm;

import java.io.IOException;
import org.apache.hadoop.io.DoubleWritable;
import org.apache.hadoop.io.FloatWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapred.MapReduceBase;
import org.apache.hadoop.mapred.Mapper;
import org.apache.hadoop.mapred.OutputCollector;
import org.apache.hadoop.mapred.Reporter;

public class LinearRegressionMapper extends MapReduceBase implements
        Mapper<LongWritable, Text, LongWritable, DoubleWritable> {

    LongWritable one = new LongWritable(1);

    @Override
    public void map(LongWritable key, Text value,
            OutputCollector<LongWritable, DoubleWritable> output,
            Reporter reporter) throws IOException {

        String line = value.toString();
        String[] features = line.split(",");

        /* Obrigado Professor Odorico e Sadi */
        output.collect(one, new DoubleWritable(new Double(features[1]))); // y
        output.collect(one, new DoubleWritable(new Double(features[6]))); // x
    }
}
