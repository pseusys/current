import { BrowserModule } from '@angular/platform-browser';
import { NgModule } from '@angular/core';
import { RouterModule, Routes } from '@angular/router';
import { FormsModule } from '@angular/forms';
import { HttpClientModule } from '@angular/common/http';

import { AppComponent } from './app.component';
import { LoginComponent } from './login/login.component';
import { MainComponent } from './main/main.component';
import { BrokersComponent } from './brokers/brokers.component';
import { ActionsComponent } from './actions/actions.component';
import { FzfComponent } from './fzf/fzf.component';

const appRoutes: Routes = [
  { path: '', component: MainComponent },
  { path: 'brokers', component: MainComponent },
  { path: 'actions', component: MainComponent },
  { path: 'login', component: LoginComponent },
  { path: '**', component: FzfComponent }
];

@NgModule({
  declarations: [
    AppComponent,
    LoginComponent,
    MainComponent,
    BrokersComponent,
    ActionsComponent,
    FzfComponent
  ],
  imports: [
    BrowserModule,
    RouterModule.forRoot(appRoutes),
    FormsModule,
    HttpClientModule
  ],
  bootstrap: [AppComponent]
})
export class AppModule { }
