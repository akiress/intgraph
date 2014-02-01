FILE *out;
out = fopen("output.txt", "w");

fprintf(out, "Vertices\n");
for (int i = 0 ; i < N ; i++)
{
    fprintf(out, "vertices[%d].p.x = %f\t", i, vertices[i].p.x);
    fprintf(out, "vertices[%d].p.y = %f\t", i, vertices[i].p.y);
    fprintf(out, "vertices[%d].p.z = %f\n", i, vertices[i].p.z);
}

fprintf(out, "Triangles\n");
for (int i = 0 ; i < M ; i++)
{
    fprintf(out, "triangles[%d].a = %d\t", i, triangles[i].a);
    fprintf(out, "triangles[%d].b = %d\t", i, triangles[i].b);
    fprintf(out, "triangles[%d].c = %d\n", i, triangles[i].c);  
}

fprintf(out, "Normals\n");
for (int i = 0 ; i < N ; i++)
{
    fprintf(out, "normals[%d].n.x = %f\t", i, vertices[i].n.x);
    fprintf(out, "normals[%d].n.y = %f\t", i, vertices[i].n.y);
    fprintf(out, "normals[%d].n.z = %f\n", i, vertices[i].n.z);
}