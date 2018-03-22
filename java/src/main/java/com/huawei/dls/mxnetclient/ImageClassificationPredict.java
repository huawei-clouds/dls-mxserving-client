package com.huawei.dls.mxnetclient;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.core.type.TypeReference;
import com.fasterxml.jackson.databind.ObjectMapper;
import net.sourceforge.argparse4j.ArgumentParsers;
import net.sourceforge.argparse4j.inf.ArgumentParser;
import net.sourceforge.argparse4j.inf.ArgumentParserException;
import net.sourceforge.argparse4j.inf.Namespace;
import org.apache.http.HttpEntity;
import org.apache.http.client.ClientProtocolException;
import org.apache.http.client.HttpClient;
import org.apache.http.client.ResponseHandler;
import org.apache.http.client.methods.HttpUriRequest;
import org.apache.http.client.methods.RequestBuilder;
import org.apache.http.entity.ContentType;
import org.apache.http.entity.mime.MultipartEntityBuilder;
import org.apache.http.impl.client.HttpClients;
import org.apache.http.util.EntityUtils;

import java.io.File;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class ImageClassificationPredict {

    private static final String SERVER_URL = "server_url";
    private static final String FILE_PATH = "file_path";
    private static final String LABEL_FILE_PATH = "label_file_path";
    private static final String INPUT_NODE_NAME = "input_node_name";

    // Process response json body
    private static Map<String, List<Map<String, String>>> convertJsonToResultMap(String content) throws IOException {
        ObjectMapper mapper = new ObjectMapper();
        TypeReference<HashMap<String, List<Map<String, String>>>> typeRef
                = new TypeReference<HashMap<String, List<Map<String, String>>>>() {
        };
        return mapper.readValue(content, typeRef);
    }

    // Parse parameters
    private static ArgumentParser getParser() {
        ArgumentParser parser = ArgumentParsers.newArgumentParser("ImageClassificationPredict")
                .defaultHelp(true)
                .description("predict");

        parser.addArgument("--" + SERVER_URL)
                .type(String.class)
                .setDefault("http://127.0.0.1:8080/squeezenet_v1.1/predict")
                .help("The server url.");

        parser.addArgument("--" + FILE_PATH)
                .type(String.class)
                .setDefault("kitten.jpg")
                .help("The pic file used to predict.");

        parser.addArgument("--" + LABEL_FILE_PATH)
                .type(String.class)
                .setDefault("synset.txt")
                .help("The label file for predict result.");

        parser.addArgument("--" + INPUT_NODE_NAME)
                .type(String.class)
                .setDefault("data")
                .help("The input node name of the model running on predict server.");

        return parser;
    }

    private static void predict(String serverUrl, String filePath, String labelFilePath, String inputNodeName) {
        File imageFile = new File(filePath);
        File labelFile = new File(labelFilePath);
        if (!imageFile.exists() || !labelFile.exists()) {
            System.err.println("Client error: image file or label file not exist");
            return;
        }

        HttpClient httpClient = HttpClients.createDefault();

        // Build request
        HttpEntity data = MultipartEntityBuilder.create()
                .addBinaryBody(inputNodeName, imageFile, ContentType.create("image/jpeg"), imageFile.getName())
                .build();

        // Assign multipart upload data
        HttpUriRequest request = RequestBuilder
                .post(serverUrl)
                .setEntity(data)
                .build();

        // Create a custom response handler
        ResponseHandler<String> responseHandler = response -> {
            int status = response.getStatusLine().getStatusCode();
            if (status >= 200 && status < 300) {
                HttpEntity entity = response.getEntity();
                return entity != null ? EntityUtils.toString(entity) : null;
            } else {
                throw new ClientProtocolException("Unexpected response status: " + status);
            }
        };

        // Extract response
        String responseBody = "";
        try {
            responseBody = httpClient.execute(request, responseHandler);
        } catch (IOException e) {
            System.err.println("Client error: " + e.getMessage());
            return;
        }

        Map<String, List<Map<String, String>>> predictMap = null;
        try {
            predictMap = convertJsonToResultMap(responseBody);
        } catch (IOException e) {
            System.err.println("Response body is invalid: " + e.getMessage());
            return;
        }

        // Get label of image
        List<String> lines = null;
        try {
            lines = Files.readAllLines(Paths.get(labelFilePath));
        } catch (IOException e) {
            System.err.println("Read label file error: " + e.getMessage());
            return;
        }

        // Extract result
        if (!predictMap.containsKey("prediction")) {
            System.err.println("Result json is invalid");
        } else {
            List<Map<String, String>> itemMapList = predictMap.get("prediction");

            for (Map<String, String> itemMap : itemMapList) {
                int classNum = Integer.parseInt(itemMap.get("class"));
                if (classNum >= lines.size()) {
                    System.err.println("Label file is invalid");
                    return;
                }
                itemMap.put("class", lines.get(classNum));
            }

            // Output result
            try {
                System.out.println(new ObjectMapper().writeValueAsString(predictMap));
            } catch (JsonProcessingException e) {
                System.err.println("Process json error: " + e.getMessage());
            }
        }
    }

    public static void main(String[] args) {
        ArgumentParser parser = getParser();
        Namespace namespace = null;

        try {
            namespace = parser.parseArgs(args);
        } catch (ArgumentParserException e) {
            System.err.println("Parameter error: " + e.getMessage());
            return;
        }

        String serverUrl = namespace.getString(SERVER_URL);
        String filePath = namespace.getString(FILE_PATH);
        String labelFilePath = namespace.getString(LABEL_FILE_PATH);
        String inputNodePath = namespace.getString(INPUT_NODE_NAME);

        predict(serverUrl, filePath, labelFilePath, inputNodePath);
    }

}
