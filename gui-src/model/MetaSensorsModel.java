package model;

import java.io.File;
import java.io.FileReader;
import java.io.IOException;

import org.json.simple.JSONArray;
import org.json.simple.JSONObject;
import org.json.simple.parser.JSONParser;
import org.json.simple.parser.ParseException;

import view.View;

public class MetaSensorsModel extends EntityModel<MetaSensorsModel.MetaSensor> {

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Singleton Construction
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Singleton instance of MetaSensorsModel class
     */
    private static final MetaSensorsModel instance = new MetaSensorsModel();

    /**
     * Return an instance of the MetaSensorsModel class
     *
     * @return An instance of the MetaSensorsModel class
     */
    public static MetaSensorsModel getInstance() { return instance; }

    /**
     * Private default constructor for singleton
     */
    private MetaSensorsModel() {}

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // MetaSensor Class Definition
    ////////////////////////////////////////////////////////////////////////////

    public class MetaSensor
    extends EntityModel<MetaSensorsModel.MetaSensor>.Entity {

        ////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////
        // Instance Variables
        ////////////////////////////////////////////////////////////////////////
        
        // id, description fields are already inherited from Entity

        ////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////
        // Other Methods
        ////////////////////////////////////////////////////////////////////////

        /**
         * Return true if the metaperson is valid
         * 
         * @return True if the metaperson is valid
         */
        public boolean isValid() {
            return id != null;
        }

    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Queries / Access Methods
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Remove a metasensor from the model
     * 
     * @param mp The metasensor to remove
     */
    public void remove(MetaSensor ms) {
        idMap.remove(ms.id);
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // I/O Methods
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Load the provided metasensors file
     * 
     * @param f The file to load
     */
    public void loadFile(File f) {
        if (f == null) {
            View.log("Error: No metasensors file selected");
            return;
        }

        try {
            JSONParser parser = new JSONParser();

            JSONArray arr = (JSONArray) parser.parse(new FileReader(f));
            for (Object element : arr) {
                JSONObject o = (JSONObject) element;

                MetaSensor ms = new MetaSensor();

                // id
                ms.setID((int) (long) o.get("id"));

                // description
                ms.setDescription((String) o.get("description"));

                this.add(ms);
            }

            View.log("Loaded MetaSensors File: " + f.getAbsolutePath());

        } 
        catch (IllegalArgumentException e) {
            View.log("Error: " + e.getMessage());
        } 
        catch (ParseException e) {
            View.log("Error: Cannot parse file: " + f.getAbsolutePath());
        } 
        catch (IOException e) {
            View.log("Error: Invalid metasensors file: " + f.getAbsolutePath());
        }
    }

    /**
     * Write the metasensors model to file
     */
    @SuppressWarnings("unchecked")
    public void writeToFile() {
        if (!checkValid(true))
            return;

        String fname = Model.getConfig().getMetaSensorsPath();

        JSONArray data = new JSONArray();

        for (MetaSensor ms : this) {
            JSONObject o = new JSONObject();

            // id
            o.put("id", ms.getID());

            // description
            o.put("description", ms.getDescription());

            data.add(o);
        }

        Model.getInstance().writeJSON(fname, data, "metasensors");

    }

}
