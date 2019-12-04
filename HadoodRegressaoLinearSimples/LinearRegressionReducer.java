package br.ufsc.rlm;

import java.util.*;
import java.io.IOException;
import org.apache.hadoop.io.DoubleWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapred.MapReduceBase;
import org.apache.hadoop.mapred.OutputCollector;
import org.apache.hadoop.mapred.Reducer;
import org.apache.hadoop.mapred.Reporter;

public class LinearRegressionReducer extends MapReduceBase
        implements Reducer<LongWritable, DoubleWritable, Text, DoubleWritable> {

    @Override
    public void reduce(LongWritable key,
            Iterator<DoubleWritable> values,
            OutputCollector<Text, DoubleWritable> output,
            Reporter reporter)
            throws IOException {

        Integer n = 0;
        double x = 0.0;
        double y = 0.0;
        double sumx = 0.0;
        double sumy = 0.0;
        double sumx2 = 0.0;
        double sumy2 = 0.0;
        double sumxy = 0.0;

        while (values.hasNext()) {
            x = values.next().get();
            sumx += x;
            sumx2 += x * x;

            y = values.next().get();
            sumy += y;
            sumy2 += y * y;
            sumxy += x * y;
            n++;
        }

        /* Obrigado Professor Nakamura */
        double x_bar = sumx / n;
        double y_bar = sumy / n;

        double b1_upper_part = sumxy - (n * x_bar * y_bar);
        double b1_lower_part = sumx2 - (n * x_bar * x_bar);

        double b1 = b1_upper_part / b1_lower_part;
        double b0 = y_bar - (b1 * x_bar);

        double r = b1_upper_part / (Math.sqrt(sumx2 - (n * x_bar * x_bar)) * Math.sqrt((sumy2) - (n * y_bar * y_bar)));

        output.collect(new Text("b1"), new DoubleWritable(b1));
        output.collect(new Text("b0"), new DoubleWritable(b0));
        output.collect(new Text("r"), new DoubleWritable(r));
    }
}
