package model;

import java.io.File;
import java.io.FileReader;
import java.io.IOException;

import org.json.simple.JSONArray;
import org.json.simple.JSONObject;
import org.json.simple.parser.JSONParser;
import org.json.simple.parser.ParseException;

import model.MetaPeopleModel.MetaPerson;
import view.View;

public class PeopleModel extends EntityModel<PeopleModel.Person> {

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Singleton Construction
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Singleton instance of PeopleModel class
     */
    private static final PeopleModel instance = new PeopleModel();

    /**
     * Return an instance of the PeopleModel class
     *
     * @return An instance of the PeopleModel class
     */
    public static PeopleModel getInstance() { return instance; }

    /**
     * Private default constructor for singleton
     */
    private PeopleModel() {}

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Person Class Definition
    ////////////////////////////////////////////////////////////////////////////

    public class Person extends EntityModel<PeopleModel.Person>.Entity {

        ////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////
        // Instance Variables
        ////////////////////////////////////////////////////////////////////////

        private Integer metaid = null;
        private String  tp     = null;

        ////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////
        // Getter Methods
        ////////////////////////////////////////////////////////////////////////

        /**
         * Return the metaperson ID
         * @return The metaperson ID
         */
        public int getMetaID() {
            return this.metaid;
        }

        /**
         * Return the name of the time profile
         * @return The name of the time profile
         */
        public String getProfileName() {
            return this.tp;
        }

        ////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////
        // Setter Methods
        ////////////////////////////////////////////////////////////////////////

        /**
         * Set the metaperson ID
         * @param id The metaperson ID
         */
        public void setMetaID(Integer id) {
            this.metaid = id;
        }

        /**
         * Set the name of the time profile
         * @param name The name of the time profile
         */
        public void setProfile(String name) {
            this.tp = name;
        }

        ////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////
        // Other Methods
        ////////////////////////////////////////////////////////////////////////

        /**
         * Return true if the person is valid
         * 
         * @return True if the person is valid
         */
        public boolean isValid() {
            return id != null && metaid != null && tp != null;
        }

    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Queries / Access Methods
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Remove a person from the model
     * 
     * @param p The person to remove
     */
    public void remove(Person p) {
        idMap.remove(p.id);
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // I/O Methods
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Load the provided people file
     * 
     * @param f The file to load
     */
    public void loadFile(File f) {
        if (f == null) {
            View.log("Error: No people file selected");
            return;
        }

        try {
            JSONParser parser = new JSONParser();

            JSONArray arr = (JSONArray) parser.parse(new FileReader(f));
            for (Object element : arr) {
                JSONObject o = (JSONObject) element;

                Person p = new Person();

                // id
                p.setID((int) (long) o.get("id"));

                // metaperson-id
                p.setMetaID((int) (long) o.get("metaperson-id"));

                // description
                p.setDescription((String) o.get("description"));

                // profile-index
                MetaPerson mp = Model.getMetaPeople().get(p.metaid);
                p.setProfile(mp.getProfileName(
                        (int) (long) o.get("profile-index")));

                this.add(p);
            }

            View.log("Loaded People File: " + f.getAbsolutePath());

        }  
        catch (IllegalArgumentException e) {
            View.log("Error: " + e.getMessage());
        } 
        catch (ParseException e) {
            View.log("Error: Cannot parse file: " + f.getAbsolutePath());
        } 
        catch (IOException e) {
            View.log("Error: Invalid people file: " + f.getAbsolutePath());
        }

    }

    /**
     * Write the people model to file
     */
    @SuppressWarnings("unchecked")
    public void writeToFile() {
        if (!checkValid(true))
            return;

        String fname = Model.getConfig().getPeoplePath();

        JSONArray data = new JSONArray();

        for (Person p : this) {
            JSONObject o = new JSONObject();

            // id
            o.put("id", p.id);

            // metaperson-id
            o.put("metaperson-id", p.metaid);

            // description
            o.put("description", p.description);

            // profile-index
            MetaPerson mp = Model.getMetaPeople().get(p.id);
            o.put("profile-index", mp.getProfileIndex(p.getProfileName()));

            data.add(o);
        }

        Model.getInstance().writeJSON(fname, data, "people");

    }

}
