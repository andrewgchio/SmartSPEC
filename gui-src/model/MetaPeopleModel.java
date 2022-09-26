package model;

import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.util.AbstractMap.SimpleEntry;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import org.json.simple.JSONArray;
import org.json.simple.JSONObject;
import org.json.simple.parser.JSONParser;
import org.json.simple.parser.ParseException;

import model.TimeProfilesModel.TimeProfile;
import view.View;

public class MetaPeopleModel extends EntityModel<MetaPeopleModel.MetaPerson> {

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Singleton Construction
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Singleton instance of MetaPeopleModel class
     */
    private static final MetaPeopleModel instance = new MetaPeopleModel();

    /**
     * Return an instance of the MetaPeopleModel class
     *
     * @return An instance of the MetaPeopleModel class
     */
    public static MetaPeopleModel getInstance() { return instance; }

    /**
     * Private default constructor for singleton
     */
    private MetaPeopleModel() {}

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // MetaPerson Class Definition
    ////////////////////////////////////////////////////////////////////////////

    public class MetaPerson 
    extends EntityModel<MetaPeopleModel.MetaPerson>.Entity {

        ////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////
        // Instance Variables
        ////////////////////////////////////////////////////////////////////////

        private Double                           pr       = null;
        private List<Entry<Double, TimeProfile>> profiles = new ArrayList<>();
        private Map<Integer, Double>             affinity = new HashMap<>();

        ////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////
        // Getter Methods
        ////////////////////////////////////////////////////////////////////////

        /**
         * Return the probability of the metaperson
         * 
         * @return The probability of the metaperson
         */
        public Double getProbability() {
            return this.pr;
        }

        /**
         * Return the time profiles of the metaperson
         * 
         * @return The time profiles of the metaperson
         */
        public List<Entry<Double, TimeProfile>> getTimeProfiles() {
            return this.profiles;
        }
        
        /**
         * Return the name of the time profile at the given index
         * 
         * @param idx The index of the time profile
         * @return The name of the time profile at the given index
         */
        public String getProfileName(int idx) {
            return profiles.get(idx).getValue().getName();
        }

        /**
         * Return the index of the time profile with the given name
         * 
         * @param name The name of the time profile
         * @return The index of the time profile with the given name
         */
        public int getProfileIndex(String name) {
            for (int i = 0; i < profiles.size(); ++i) {
                if (profiles.get(i).getValue().getName().equals(name))
                    return i;
            }

            throw new IllegalArgumentException(
                    "Error: profile index with name " + name + " not found");
        }

        /**
         * Return the event affinity of the metaperson
         * 
         * @return The event affinity of the metaperson
         */
        public Map<Integer, Double> getEventAffinity() {
            return this.affinity;
        }

        ////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////
        // Setter Methods
        ////////////////////////////////////////////////////////////////////////

        /**
         * Set the probability of the metaperson
         * 
         * @param pr The probability
         */
        public void setProbability(Double pr) {
            this.pr = pr;
        }

        /**
         * Add a time profile with given probability
         * 
         * @param pr          The probability to add
         * @param timeProfile The time profile to add
         */
        public void addTimeProfile(Double pr, TimeProfile timeProfile) {
            profiles.add(new SimpleEntry<>(pr, timeProfile));
        }

        /**
         * Add an affinity to an event
         * 
         * @param meid The metaevent ID to add
         * @param pr   The probability to add
         */
        public void addEventAffinity(Integer meid, Double pr) {
            affinity.put(meid, pr);
        }

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
            return id != null && pr != null && 
                    !profiles.isEmpty() && !affinity.isEmpty();
        }

    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Queries / Access Methods
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Remove a metaperson from the model
     * 
     * @param mp The metaperson to remove
     */
    public void remove(MetaPerson mp) {
        // remove from metaevents
        Model.getMetaEvents().stream()
                .forEach(x -> x.getCapacity().remove(mp.id));

        // remove metaperson
        idMap.remove(mp.id);
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // I/O Methods
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Load the provided metapeople file
     * 
     * @param f The file to load
     */
    public void loadFile(File f) {
        if (f == null) {
            View.log("Error: No metapeople file selected");
            return;
        }

        try {
            JSONParser parser = new JSONParser();

            JSONArray arr = (JSONArray) parser.parse(new FileReader(f));
            for (Object element : arr) {
                JSONObject o = (JSONObject) element;

                MetaPerson mp = new MetaPerson();

                // id
                mp.setID((int) (long) o.get("id"));

                // description
                mp.setDescription((String) o.get("description"));

                // probability
                mp.setProbability((double) o.get("probability"));

                // time-profiles
                JSONArray timeProfiles = (JSONArray) o.get("time-profiles");
                for (Object timeProfile : timeProfiles) {
                    JSONObject tpEntry = (JSONObject) timeProfile;

                    Double pr = (double) tpEntry.get("probability");

                    JSONArray   profile = (JSONArray) tpEntry.get("profile");
                    TimeProfile tp      = TimeProfilesModel.loadJSON(profile);

                    mp.addTimeProfile(pr, tp);
                }

                // event-affinity
                JSONArray eventAffinity = (JSONArray) o.get("event-affinity");
                for (Object element2 : eventAffinity) {
                    JSONObject eaEntry = (JSONObject) element2;
                    mp.addEventAffinity(
                            (int) (long) eaEntry.get("metaevent-id"), 
                            (double) eaEntry.get("probability"));
                }

                this.add(mp);
            }

            View.log("Loaded MetaPeople File: " + f.getAbsolutePath());

        } 
        catch (IllegalArgumentException e) {
            View.log("Error: " + e.getMessage());
        } 
        catch (ParseException e) {
            View.log("Error: Cannot parse file: " + f.getAbsolutePath());
        } 
        catch (IOException e) {
            View.log("Error: Invalid metapeople file: " + f.getAbsolutePath());
        }
    }

    /**
     * Write the metapeople model to file
     */
    @SuppressWarnings("unchecked")
    public void writeToFile() {
        if (!checkValid(true))
            return;

        String fname = Model.getConfig().getMetaPeoplePath();

        JSONArray data = new JSONArray();

        for (MetaPerson mp : this) {
            JSONObject o = new JSONObject();

            // id
            o.put("id", mp.getID());

            // description
            o.put("description", mp.getDescription());

            // probability
            o.put("probability", mp.getProbability());

            // time-profiles
            JSONArray timeProfiles = new JSONArray();
            for (Entry<Double, TimeProfile> e : mp.getTimeProfiles()) {
                JSONObject tpEntry = new JSONObject();
                tpEntry.put("probability", e.getKey());
                tpEntry.put("profile", 
                        TimeProfilesModel.writeJSON(e.getValue()));

                timeProfiles.add(tpEntry);
            }
            o.put("time-profiles", timeProfiles);

            // event-affinity
            JSONArray eventAffinity = new JSONArray();
            for (Entry<Integer, Double> e : mp.getEventAffinity().entrySet()) {
                JSONObject eaEntry = new JSONObject();
                eaEntry.put("metaevent-id", e.getKey());
                eaEntry.put("probability", e.getValue());
                eventAffinity.add(eaEntry);
            }
            o.put("event-affinity", eventAffinity);

            data.add(o);
        }

        Model.getInstance().writeJSON(fname, data, "metapeople");

    }

}
