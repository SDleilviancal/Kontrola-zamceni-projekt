package com.example.firebase_tlak_sen  // Definuje název balíčku.

import android.os.Bundle  // Importuje třídu Bundle pro práci s daty.
import android.widget.Toast  // Importuje třídu Toast pro zobrazení upozornění.
import androidx.activity.enableEdgeToEdge  // Importuje metodu pro povolení edge-to-edge layoutu.
import androidx.appcompat.app.AppCompatActivity  // Importuje třídu AppCompatActivity.
import androidx.core.view.ViewCompat  // Importuje třídu ViewCompat pro práci s kompatibilitou rozhraní.
import androidx.core.view.WindowInsetsCompat  // Importuje třídu WindowInsetsCompat pro práci s kompatibilitou okrajů okna.
import com.example.firebase_tlak_sen.databinding.ActivityMainBinding  // Importuje třídu ActivityMainBinding pro použití databindingu.
import com.google.firebase.database.*  // Importuje třídy pro práci s Firebase Realtime Database.

class MainActivity : AppCompatActivity() {  // Definuje třídu MainActivity, která dědí z AppCompatActivity.
    private lateinit var binding: ActivityMainBinding  // Inicializuje lateinit proměnnou pro použití databindingu.
    private lateinit var database: DatabaseReference  // Inicializuje lateinit proměnnou pro referenci na databázi.
    private lateinit var valueListener: ValueEventListener  // Inicializuje lateinit proměnnou pro naslouchání změnám hodnot v databázi.

    override fun onCreate(savedInstanceState: Bundle?) {  // Definuje metodu onCreate(), která se volá při vytvoření aktivity.
        super.onCreate(savedInstanceState)  // Volá metodu rodičovské třídy.
        enableEdgeToEdge()  // Povoluje edge-to-edge layout.
        binding = ActivityMainBinding.inflate(layoutInflater)  // Inicializuje databinding layout aktivity.
        setContentView(binding.root)  // Nastavuje layout aktivity.

        ViewCompat.setOnApplyWindowInsetsListener(binding.main) { v, insets ->  // Nastavuje posluchač okrajů okna.
            val systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars())  // Získává okraje systémových prvků.
            v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom)  // Nastavuje odsazení obsahu podle okrajů.
            insets  // Vrací zpracované okraje.
        }

        database = FirebaseDatabase.getInstance().reference.child("Sensor").child("fsrValue")  // Inicializuje referenci na konkrétní položku v databázi.
        valueListener = object : ValueEventListener {  // Vytváří anonymní třídu pro naslouchání změnám hodnot v databázi.
            override fun onDataChange(dataSnapshot: DataSnapshot) {  // Definuje metodu pro zpracování změn dat v databázi.
                val fst_value = dataSnapshot.getValue(Float::class.java)  // Získává hodnotu napětí ze snapshotu.
                fst_value?.let {  // Kontroluje, zda je hodnota fsr platná.
                    val locked = it > 1000  // Určuje, zda je uzamčeno na základě hodnoty fsr.
                    val statusText = if (locked) "Zamknuto" else "Odemčeno"  // Nastavuje textový stav na základě uzamčení.
                    binding.textViewVolts.text = statusText  // Nastavuje text v textovém poli.
                    val toastText = if (locked) "Successful - Zamknuto" else "Successful - Odemčeno"  // Nastavuje text upozornění.
                    Toast.makeText(this@MainActivity, toastText, Toast.LENGTH_SHORT).show()  // Zobrazuje upozornění.
                } ?: run {  // Pokud je hodnota null.
                    Toast.makeText(this@MainActivity, "Voltage path does not exist", Toast.LENGTH_SHORT).show()  // Zobrazuje upozornění o neexistenci cesty k napětí.
                }
            }

            override fun onCancelled(databaseError: DatabaseError) {  // Definuje metodu pro zpracování zrušení akce v databázi.
                Toast.makeText(this@MainActivity, "FAILED", Toast.LENGTH_SHORT).show()  // Zobrazuje upozornění o selhání.
            }
        }

        database.addValueEventListener(valueListener)  // Přidává posluchač změn hodnot v databázi.
    }

    override fun onDestroy() {  // Definuje metodu onDestroy(), která se volá při zničení aktivity.
        super.onDestroy()  // Volá metodu rodičovské třídy.
        database.removeEventListener(valueListener)  // Ruší naslouchání změnám hodnot v databázi.
    }
}
