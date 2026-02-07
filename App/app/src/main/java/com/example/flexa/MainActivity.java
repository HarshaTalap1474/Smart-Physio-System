package com.example.flexa; // Change to your actual package name

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import android.graphics.Color;
import android.os.Bundle;
import android.widget.TextView;
import android.widget.Toast;

import com.google.firebase.database.DataSnapshot;
import com.google.firebase.database.DatabaseError;
import com.google.firebase.database.DatabaseReference;
import com.google.firebase.database.FirebaseDatabase;
import com.google.firebase.database.ValueEventListener;

public class MainActivity extends AppCompatActivity {

    // UI Components
    private TextView txtEmergency, txtPostureValue, txtPostureStatus, txtJointValue;

    // Firebase References
    private DatabaseReference dbRef;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Initialize UI Elements
        txtEmergency = findViewById(R.id.txtEmergency);
        txtPostureValue = findViewById(R.id.txtPostureValue);
        txtPostureStatus = findViewById(R.id.txtPostureStatus);
        txtJointValue = findViewById(R.id.txtJointValue);

        // Initialize Firebase
        // NOTE: Ensure your google-services.json is in the /app folder
        // Use your specific URL inside getInstance()
        dbRef = FirebaseDatabase.getInstance("https://flexa-22635-default-rtdb.firebaseio.com/").getReference();

        // Start Listening for Data
        listenForData();
    }

    private void listenForData() {

        // 1. Listen for Posture Angle (MPU6050)
        dbRef.child("PostureAngle").addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(@NonNull DataSnapshot snapshot) {
                if (snapshot.exists()) {
                    String angleStr = snapshot.getValue().toString();
                    float angle = Float.parseFloat(angleStr);

                    txtPostureValue.setText(angleStr + "°");

                    // Logic: Alert if posture is bad (e.g., > 30 degrees tilt)
                    if (angle > 30) {
                        txtPostureStatus.setText("Status: INCORRECT!");
                        txtPostureStatus.setTextColor(Color.RED);
                    } else {
                        txtPostureStatus.setText("Status: Good");
                        txtPostureStatus.setTextColor(Color.parseColor("#4CAF50")); // Green
                    }
                }
            }
            @Override
            public void onCancelled(@NonNull DatabaseError error) {}
        });

        // 2. Listen for Joint Angle (Flex Sensor)
        dbRef.child("JointAngle").addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(@NonNull DataSnapshot snapshot) {
                if (snapshot.exists()) {
                    String flexStr = snapshot.getValue().toString();
                    txtJointValue.setText(flexStr + "°");
                }
            }
            @Override
            public void onCancelled(@NonNull DatabaseError error) {}
        });

        // 3. Listen for Emergency Status
        dbRef.child("Emergency").addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(@NonNull DataSnapshot snapshot) {
                if (snapshot.exists()) {
                    int status = Integer.parseInt(snapshot.getValue().toString());

                    if (status == 1) {
                        // EMERGENCY TRIGGERED
                        txtEmergency.setText("!!! EMERGENCY STOP !!!");
                        txtEmergency.setBackgroundColor(Color.RED);
                        Toast.makeText(MainActivity.this, "EMERGENCY STOP PRESSED", Toast.LENGTH_LONG).show();
                    } else {
                        // SYSTEM SAFE
                        txtEmergency.setText("SYSTEM SAFE");
                        txtEmergency.setBackgroundColor(Color.parseColor("#4CAF50")); // Green
                    }
                }
            }
            @Override
            public void onCancelled(@NonNull DatabaseError error) {}
        });
    }
}