import { ComponentFixture, TestBed } from '@angular/core/testing';

import { FzfComponent } from './fzf.component';

describe('FzfComponent', () => {
  let component: FzfComponent;
  let fixture: ComponentFixture<FzfComponent>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      declarations: [ FzfComponent ]
    })
    .compileComponents();
  });

  beforeEach(() => {
    fixture = TestBed.createComponent(FzfComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
